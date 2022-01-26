#include "CodeService/FormatElement/RangeFormatContext.h"
#include "CodeService/FormatElement/TextElement.h"
#include <string_view>

RangeFormatContext::RangeFormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options,
                                       LuaFormatRange validRange)
	: SerializeContext(parser, options),
	  _validRange(validRange),
	  _formattedRange(validRange),
	  _inValidRange(false)

{
}

void RangeFormatContext::Print(TextElement& textElement)
{
	int startOffset = textElement.GetTextRange().StartOffset;
	int startLine = _parser->GetLine(startOffset);
	int endLine = _parser->GetLine(textElement.GetTextRange().EndOffset);
	if (startLine > _validRange.EndLine || endLine < _validRange.StartLine)
	{
		_inValidRange = false;
		_characterCount += textElement.GetText().size();
		return;
	}

	_inValidRange = true;
	auto& indentState = _indentStack.back();
	if (_characterCount < indentState.Indent)
	{
		PrintIndent(indentState.Indent - _characterCount, indentState.IndentStyle);
	}
	_os << textElement.GetText();
	_characterCount += textElement.GetText().size();

	if(startLine < _formattedRange.StartLine)
	{
		_formattedRange.StartLine = startLine;
		_formattedRange.StartCharacter = _parser->GetColumn(startOffset);
	}

	if(endLine > _formattedRange.EndLine)
	{
		_formattedRange.EndLine = endLine;
	}
}

void RangeFormatContext::PrintBlank(std::size_t blank)
{
	for (std::size_t i = 0; i < blank; i++)
	{
		_characterCount++;
	}
	if (_inValidRange)
	{
		for (std::size_t i = 0; i < blank; i++)
		{
			_os << ' ';
		}
	}
}

void RangeFormatContext::PrintLine(std::size_t line)
{
	_characterCount = 0;
	if (_inValidRange)
	{
		for (int i = 0; i < line; i++)
		{
			_os << _options.end_of_line;
		}
	}
}

std::string RangeFormatContext::GetText()
{
	std::string formattedText = SerializeContext::GetText();

	for (int i = static_cast<int>(formattedText.size()) - 1; i >= 0; i--)
	{
		char ch = formattedText[i];

		if (ch != '\n' && ch != '\r')
		{
			formattedText = formattedText.substr(0, i + 1).append(_options.end_of_line);
			break;
		}
	}

	return formattedText;
}

LuaFormatRange RangeFormatContext::GetFormattedRange()
{
	return _formattedRange;
}
