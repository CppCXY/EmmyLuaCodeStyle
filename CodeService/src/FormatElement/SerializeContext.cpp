#include "CodeService/FormatElement/SerializeContext.h"
#include "CodeService/FormatElement/TextElement.h"

SerializeContext::SerializeContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options)
	: FormatContext(parser, options)
{
}

void SerializeContext::Print(TextElement& textElement)
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
	_buffer.append(textElement.GetText());
	_characterCount += textElement.GetText().size();
}

void SerializeContext::PrintLine(int line)
{
	if (line <= 0)
	{
		return;
	}
	for (int i = 0; i < line; i++)
	{
		_buffer.append(_options.end_of_line);
		_characterCount = 0;
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
	return std::move(_buffer);
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
