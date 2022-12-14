#include "ProtocolBuffer.h"
#include <cstring>
#include <algorithm>

ProtocolBuffer::ProtocolBuffer(std::size_t capacity)
	: _writeIndex(0),
	  // _readBuffer(capacity, 0),
	  _textProtocol(capacity, 0),
	  _contentLength(0),
	  _bodyStartIndex(0),
	  _normalCapacity(capacity)
{
}

char* ProtocolBuffer::GetWritableCursor()
{
	return _textProtocol.data() + _writeIndex;
}


std::size_t ProtocolBuffer::GetRestCapacity()
{
	return _textProtocol.size() - _writeIndex;
}

void ProtocolBuffer::SetWriteSize(std::size_t size)
{
	_writeIndex += size;
}

bool ProtocolBuffer::CanReadOneProtocol()
{
	if (_writeIndex == 0)
	{
		return false;
	}

	bool success = TryParseHead();

	return success && (_contentLength + _bodyStartIndex <= _writeIndex);
}

std::string_view ProtocolBuffer::ReadOneProtocol()
{
	if (_contentLength != 0 && _bodyStartIndex != 0)
	{
		if (_writeIndex >= _contentLength + _bodyStartIndex)
		{
			return std::string_view(_textProtocol.data() + _bodyStartIndex, _contentLength);
		}
	}
	return "";
}

void ProtocolBuffer::FitCapacity()
{
	auto oneProtocolCapacity = _contentLength + _bodyStartIndex;
	if (oneProtocolCapacity > _normalCapacity && oneProtocolCapacity > _textProtocol.size()) 
	{
		_textProtocol.resize(oneProtocolCapacity);
	}
}

void ProtocolBuffer::Reset()
{
	if (_writeIndex > _contentLength + _bodyStartIndex)
	{
		std::size_t doneIndex = _contentLength + _bodyStartIndex;
		std::copy_n(_textProtocol.data() + doneIndex, _writeIndex - doneIndex, _textProtocol.data());

		_writeIndex -= doneIndex;
		if (_writeIndex > _normalCapacity)
		{
			_textProtocol.resize(_writeIndex);
			_textProtocol.shrink_to_fit();
		}
		else
		{
			_textProtocol.resize(_normalCapacity);
			_textProtocol.shrink_to_fit();
		}
	}
	else
	{
		_writeIndex = 0;
		if (_textProtocol.size() > _normalCapacity)
		{
			_textProtocol.resize(_normalCapacity);
			_textProtocol.shrink_to_fit();
		}
	}

	_contentLength = 0;
	_bodyStartIndex = 0;
}

bool ProtocolBuffer::TryParseHead()
{
	enum class ParseState
	{
		HeadStart,
		CRLF,
		CRLF_CRLF
	} state = ParseState::HeadStart;

	std::string_view text(_textProtocol.data(), _writeIndex);
	for (std::size_t index = 0; index < text.size();)
	{
		switch (state)
		{
		case ParseState::HeadStart:
			{
				std::size_t colonPosition = text.find_first_of(":", index);
				if (colonPosition == std::string_view::npos)
				{
					return false;
				}

				std::size_t lineEndPosition = text.find_first_of("\r\n", colonPosition);
				if (lineEndPosition == std::string_view::npos)
				{
					return false;
				}

				if (text.substr(index, colonPosition - index) == "Content-Length")
				{
					if (lineEndPosition < colonPosition + 1)
					{
						return false;
					}
					auto lengthStr = text.substr(colonPosition + 1, lineEndPosition - colonPosition - 1);
					_contentLength = std::stoi(std::string(lengthStr));
					
				}
				index = lineEndPosition + 2;
				_bodyStartIndex = index + 2;
				state = ParseState::CRLF;
				break;
			}
		case ParseState::CRLF:
			{
				if ((text.size() - 1 > index) && text[index] == '\r' && text[index + 1] == '\n')
				{
					index += 2;
					state = ParseState::CRLF_CRLF;
				}
				else
				{
					state = ParseState::HeadStart;
				}
				break;
			}
		case ParseState::CRLF_CRLF:
			{
				_bodyStartIndex = index;
				return true;
			}
		}
	}

	return false;
}
