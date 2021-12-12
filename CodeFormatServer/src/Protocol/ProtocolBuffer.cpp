#include "CodeFormatServer/Protocol/ProtocolBuffer.h"
#include <cstring>

ProtocolBuffer::ProtocolBuffer(std::size_t capacity)
	: _writeIndex(0),
	  _readBuffer(capacity, 0),
	  _contentLength(0),
	  _bodyStartIndex(0)
{
}

char* ProtocolBuffer::GetWritableCursor()
{
	return _readBuffer.data();
}


std::size_t ProtocolBuffer::GetRestCapacity()
{
	return _readBuffer.size();
}

void ProtocolBuffer::WriteBuff(std::size_t size)
{
	if (_textProtocol.size() < _writeIndex + size)
	{
		if (_bodyStartIndex != 0 && _contentLength != 0 && _writeIndex < (_bodyStartIndex + _contentLength))
		{
			_textProtocol.resize(std::max(_writeIndex + size, _bodyStartIndex + _contentLength));
		}
		else
		{
			_textProtocol.resize(_writeIndex + size);
		}
	}

	std::copy_n(_readBuffer.begin(), size, _textProtocol.begin() + _writeIndex);
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

void ProtocolBuffer::Reset()
{
	if (_writeIndex > _contentLength + _bodyStartIndex)
	{
		std::size_t doneIndex = _contentLength + _bodyStartIndex;
		std::copy_n(_textProtocol.data() + doneIndex, _writeIndex - doneIndex, _textProtocol.data());

		_writeIndex -= doneIndex;
		_textProtocol.resize(std::max(_writeIndex, _readBuffer.size()));
		_textProtocol.shrink_to_fit();
	}
	else
	{
		_writeIndex = 0;
		_textProtocol.clear();
		_textProtocol.resize(_readBuffer.size());
		_textProtocol.shrink_to_fit();
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
