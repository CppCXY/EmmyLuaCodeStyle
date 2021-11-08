#include "CodeFormatServer/Protocol/ProtocolBuffer.h"
#include <cstring>

ProtocolBuffer::ProtocolBuffer(std::size_t capacity)
	: _writeIndex(0),
	  _startIndex(0),
	  _contentLength(0),
	  _bodyStartIndex(0)
{
	_buffer.resize(capacity);
}

char* ProtocolBuffer::GetWritableCursor()
{
	return _buffer.data() + _writeIndex;
}


std::size_t ProtocolBuffer::GetRestCapacity()
{
	return _buffer.size() - _writeIndex;
}

void ProtocolBuffer::SetReadableSize(std::size_t readableSize)
{
	_writeIndex = _startIndex + readableSize;
}

bool ProtocolBuffer::CanReadOneProtocol()
{
	if (_startIndex == _writeIndex)
	{
		return false;
	}


	bool success = TryParseHead();
	bool completeOneProtocol = false;

	if (success)
	{
		completeOneProtocol = _contentLength + _bodyStartIndex <= _writeIndex;
	}

	// 如果头解析不完整或者协议接收不完整
	// 试图扩容
	if ((!success || !completeOneProtocol) && (_buffer.size() - _writeIndex <= 1))
	{
		if (_contentLength != 0 && _bodyStartIndex != 0)
		{
			_buffer.resize(_bodyStartIndex + _contentLength);
		}
		else
		{
			_buffer.resize(_buffer.size() * 2);
		}
	}

	return completeOneProtocol;
}

std::string_view ProtocolBuffer::ReadOneProtocol()
{
	if (_contentLength != 0 && _bodyStartIndex != 0)
	{
		if (_writeIndex >= _contentLength + _bodyStartIndex)
		{
			return std::string_view(_buffer.data() + _bodyStartIndex, _contentLength);
		}
	}
	return "";
}

void ProtocolBuffer::Reset()
{
	if(_writeIndex > _contentLength + _bodyStartIndex)
	{
		std::size_t doneIndex = _contentLength + _bodyStartIndex;
		std::memmove(_buffer.data(), _buffer.data() + doneIndex, _writeIndex - doneIndex);

		_startIndex = 0;
		_writeIndex -= _contentLength + _bodyStartIndex;
	}
	else
	{
		_startIndex = 0;
		_writeIndex = 0;
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

	std::string_view text(_buffer.data() + _startIndex, _writeIndex - _startIndex);
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

	return true;
}
