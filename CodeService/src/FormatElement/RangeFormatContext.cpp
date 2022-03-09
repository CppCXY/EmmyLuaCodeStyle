#include "CodeService/FormatElement/RangeFormatContext.h"


RangeFormatContext::RangeFormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options,
                                       LuaFormatRange validRange)
	: SerializeContext(parser, options),
	  _validRange(validRange),
	  _formattedRange(validRange),
	  _inValidRange(false)

{
}

void RangeFormatContext::Print(std::string_view text, TextRange range)
{
	int startOffset = range.StartOffset;
	int startLine = _parser->GetLine(startOffset);
	int endLine = _parser->GetLine(range.EndOffset);
	if (startLine > _validRange.EndLine || endLine < _validRange.StartLine)
	{
		_inValidRange = false;
		_characterCount += text.size();
		return;
	}

	_inValidRange = true;
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
	InnerPrintText(text, range);

	if (startLine < _formattedRange.StartLine)
	{
		_formattedRange.StartLine = startLine;
		_formattedRange.StartCharacter = _parser->GetColumn(startOffset);
	}

	if (endLine > _formattedRange.EndLine)
	{
		_formattedRange.EndLine = endLine;
	}
}

void RangeFormatContext::Print(char ch, int offset)
{
	int startLine = _parser->GetLine(offset);
	int endLine = _parser->GetLine(offset);
	if (startLine > _validRange.EndLine || endLine < _validRange.StartLine)
	{
		_inValidRange = false;
		_characterCount += 1;
		return;
	}

	_inValidRange = true;
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

	if (startLine < _formattedRange.StartLine)
	{
		_formattedRange.StartLine = startLine;
		_formattedRange.StartCharacter = _parser->GetColumn(offset);
	}

	if (endLine > _formattedRange.EndLine)
	{
		_formattedRange.EndLine = endLine;
	}
}

void RangeFormatContext::PrintBlank(int blank)
{
	for (int i = 0; i < blank; i++)
	{
		_characterCount++;
	}
	if (_inValidRange)
	{
		for (int i = 0; i < blank; i++)
		{
			_buffer.push_back(' ');
		}
	}
}

void RangeFormatContext::PrintLine(int line)
{
	_characterCount = 0;
	if (_inValidRange)
	{
		for (int i = 0; i < line; i++)
		{
			_buffer.append(_options.end_of_line);
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
