#include "CodeService/FormatElement/RangeFormatContext.h"
#include "CodeService/FormatElement/TextElement.h"
#include <string_view>

RangeFormatContext::RangeFormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options,
                                       LuaFormatRange validRange)
	: FormatContext(parser, options),
	  _validRange(validRange),
	  _formattedLine(-1)

{
}

void RangeFormatContext::Print(TextElement& textElement)
{
	int offset = textElement.GetTextRange().StartOffset;
	int offsetLine = _parser->GetLine(offset);
	if (offsetLine < _validRange.StartLine || offsetLine > _validRange.EndLine)
	{
		_characterCount += textElement.GetText().size();
		_formattedLine = -1;
		return;
	}

	auto& indentState = _indentStack.top();
	if (static_cast<int>(_characterCount) < indentState.Indent)
	{
		PrintIndent(indentState.Indent - static_cast<int>(_characterCount), indentState.IndentString);
	}
	_os << textElement.GetText();
	_characterCount += textElement.GetText().size();

	_formattedLine = _parser->GetLine(textElement.GetTextRange().EndOffset);
}

void RangeFormatContext::PrintBlank(int blank)
{
	for (int i = 0; i < blank; i++)
	{
		_characterCount++;
	}
	if (_formattedLine >= 0)
	{
		for (int i = 0; i < blank; i++)
		{
			_os << ' ';
		}
	}
}

void RangeFormatContext::PrintLine(int line)
{
	_characterCount = 0;
	if (_formattedLine >= 0 && _formattedLine <= _validRange.EndLine)
	{
		for (int i = 0; i < line; i++)
		{
			_os << _options.line_separator;
		}
	}
}

std::string RangeFormatContext::GetText()
{
	std::string formattedText = FormatContext::GetText();

	for (int i = formattedText.size() - 1; i >= 0; i--)
	{
		char ch = formattedText[i];

		if (ch != '\n' && ch != '\r')
		{
			formattedText = formattedText.substr(0, i + 1).append(_options.line_separator);
			break;
		}
	}

	return formattedText;
}

LuaFormatRange RangeFormatContext::GetFormattedRange()
{
	return _validRange;
}
