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
		if (_characterCount < indentState.Indent)
		{
			PrintIndent(indentState.Indent - _characterCount, indentState.IndentStyle);
		}
	}
	_os << textElement.GetText();
	_characterCount += textElement.GetText().size();
}

void SerializeContext::PrintLine(std::size_t line)
{
	for (std::size_t i = 0; i < line; i++)
	{
		_os << _options.end_of_line;
		_characterCount = 0;
	}
}

void SerializeContext::PrintBlank(std::size_t blank)
{
	for (std::size_t i = 0; i < blank; i++)
	{
		_os << ' ';
		_characterCount++;
	}
}

std::string SerializeContext::GetText()
{
	return _os.str();
}

void SerializeContext::PrintIndent(std::size_t indent, IndentStyle style)
{
	switch (style)
	{
	case IndentStyle::Space:
		{
			for (std::size_t i = 0; i < indent; i++)
			{
				_os << ' ';
			}
			_characterCount += indent;
			break;
		}
	case IndentStyle::Tab:
		{
			for (std::size_t i = 0; i < indent; i++)
			{
				_os << '\t';
			}
			_characterCount += indent;
			break;
		}
	}
}
