#include "CodeService/FormatElement/RangeFormatContext.h"
#include "CodeService/FormatElement/TextElement.h"

RangeFormatContext::RangeFormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options,
                                       LuaFormatRange validRange)
	: FormatContext(parser, options),
	  _validRange(validRange),
	  _inFormattedRange(false)

{
}

void RangeFormatContext::Print(TextElement& textElement)
{
	int offset = textElement.GetTextRange().StartOffset;
	int offsetLine = _parser->GetLine(offset);
	if (offsetLine < _validRange.StartLine || offsetLine > _validRange.EndLine)
	{
		_characterCount += textElement.GetText().size();
		_inFormattedRange = false;
		return;
	}

	_inFormattedRange = true;

	auto& indentState = _indentStack.top();
	if (static_cast<int>(_characterCount) < indentState.Indent)
	{
		PrintIndent(indentState.Indent - static_cast<int>(_characterCount), indentState.IndentString);
	}
	_os << textElement.GetText();
	_characterCount += textElement.GetText().size();

	int endOffsetLine = _parser->GetLine(textElement.GetTextRange().EndOffset);
	if (endOffsetLine > _validRange.EndLine)
	{
		_validRange.EndLine = endOffsetLine;
	}
}

void RangeFormatContext::PrintBlank(int blank)
{
	for (int i = 0; i < blank; i++)
	{
		_characterCount++;
	}
	if (_inFormattedRange)
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
	if (_inFormattedRange)
	{
		for (int i = 0; i < line; i++)
		{
			_os << _options.line_separator;
		}
	}
}

LuaFormatRange RangeFormatContext::GetFormattedRange()
{
	return _validRange;
}
