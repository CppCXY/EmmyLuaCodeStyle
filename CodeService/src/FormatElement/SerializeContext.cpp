#include "CodeService/FormatElement/SerializeContext.h"

SerializeContext::SerializeContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options)
	: FormatContext(parser, options),
	  _luaFile(parser->GetLuaFile())
{
}

void SerializeContext::Print(std::string_view text, TextRange range)
{
	PrintIndentOnly();
	InnerPrintText(text, range);
}

void SerializeContext::Print(char ch, int Offset)
{
	if (!_indentStack.empty())
	{
		auto& indentState = _indentStack.back();
		if (indentState.Style == IndentStyle::Space)
		{
			if (_characterCount < indentState.SpaceIndent)
			{
				PrintIndent(indentState.SpaceIndent - _characterCount, indentState.Style);
			}
		}
		else
		{
			if (_characterCount == 0)
			{
				PrintIndent(indentState.TabIndent, indentState.Style);
				PrintIndent(indentState.SpaceIndent, IndentStyle::Space);
			}
			else if (_characterCount >= indentState.TabIndent && (indentState.SpaceIndent + indentState.TabIndent >
				_characterCount))
			{
				PrintIndent(indentState.SpaceIndent - (_characterCount - indentState.TabIndent), IndentStyle::Space);
			}
		}
	}
	_buffer.push_back(ch);
	_characterCount++;
}

void SerializeContext::PrintLine(int line)
{
	if (line <= 0)
	{
		return;
	}
	for (int i = 0; i < line; i++)
	{
		_characterCount = 0;
		PrintEndOfLine();
	}
}

void SerializeContext::PrintBlank(int blank)
{
	if (blank <= 0)
	{
		return;
	}
	for (int i = 0; i < blank; i++)
	{
		_buffer.push_back(' ');
		_characterCount++;
	}
}

std::string SerializeContext::GetText()
{
	if (!_options.insert_final_newline && !_buffer.empty())
	{
		std::size_t validSize = _buffer.size();
		for (; validSize >= 1; validSize--)
		{
			char ch = _buffer[validSize - 1];
			if (ch != '\r' && ch != '\n')
			{
				break;
			}
		}
		if (validSize > 0 && validSize <= _buffer.size())
		{
			_buffer.resize(validSize);
		}
	}

	return std::move(_buffer);
}

void SerializeContext::PrintIndentOnly(int line)
{
	(void)line;

	if (!_indentStack.empty())
	{
		auto& indentState = _indentStack.back();
		if (indentState.Style == IndentStyle::Space)
		{
			if (_characterCount < indentState.SpaceIndent)
			{
				PrintIndent(indentState.SpaceIndent - _characterCount, indentState.Style);
			}
		}
		else
		{
			if (_characterCount == 0)
			{
				PrintIndent(indentState.TabIndent, indentState.Style);
				PrintIndent(indentState.SpaceIndent, IndentStyle::Space);
			}
			else if (_characterCount >= indentState.TabIndent && (indentState.SpaceIndent + indentState.TabIndent >
				_characterCount))
			{
				PrintIndent(indentState.SpaceIndent - (_characterCount - indentState.TabIndent), IndentStyle::Space);
			}
		}
	}
}

void SerializeContext::SetReadySize(std::size_t size)
{
	_buffer.reserve(size);
}

void SerializeContext::PrintIndent(std::size_t indent, IndentStyle style)
{
	switch (style)
	{
	case IndentStyle::Space:
		{
			for (std::size_t i = 0; i < indent; i++)
			{
				_buffer.push_back(' ');
			}
			_characterCount += indent;
			break;
		}
	case IndentStyle::Tab:
		{
			for (std::size_t i = 0; i < indent; i++)
			{
				_buffer.push_back('\t');
			}
			_characterCount += indent;
			break;
		}
	}
}

void SerializeContext::InnerPrintText(std::string_view text, TextRange range)
{
	if (GetLine(range.StartOffset) == GetLine(range.EndOffset))
	{
		_buffer.append(text);
		_characterCount += text.size();
		return;
	}

	std::size_t lastStartIndex = 0;

	for (std::size_t i = 0; i < text.size(); i++)
	{
		char ch = text[i];

		if (ch == '\r' || ch == '\n')
		{
			if (i - lastStartIndex != 0)
			{
				_buffer.append(text.substr(lastStartIndex, i - lastStartIndex));
			}
			PrintEndOfLine();
			lastStartIndex = i + 1;
			if (ch == '\r' && lastStartIndex < text.size() && text[lastStartIndex] == '\n')
			{
				lastStartIndex++;
				i++;
			}
		}
	}

	if (lastStartIndex < text.size())
	{
		_buffer.append(text.substr(lastStartIndex, text.size() - lastStartIndex));
	}
	_characterCount = GetColumn(range.EndOffset);
}

void SerializeContext::PrintEndOfLine()
{
	if (_options.detect_end_of_line)
	{
		auto endOfLine = _luaFile->GetEndOfLine();
		switch (endOfLine)
		{
		case EndOfLine::CRLF:
		case EndOfLine::LF:
		case EndOfLine::CR:
			{
				InnerPrintEndOfLine(_buffer, endOfLine);
				return;
			}
		default:
			{
				break;
			}
		}
	}

	InnerPrintEndOfLine(_buffer, _options.end_of_line);
}

void SerializeContext::InnerPrintEndOfLine(std::string& buffer, EndOfLine endOfLine)
{
	switch (endOfLine)
	{
	case EndOfLine::LF:
		{
			buffer.push_back('\n');
			break;;
		}
	case EndOfLine::CRLF:
		{
			buffer.push_back('\r');
			buffer.push_back('\n');
			break;
		}
	case EndOfLine::CR:
		{
			buffer.push_back('\r');
			break;
		}
	default:
		{
			break;
		}
	}
}
