#include "LuaParser/TextRange.h"

TextRange::TextRange(int startOffset, int endOffset)
	: StartOffset(startOffset),
	  EndOffset(endOffset)
{
}

bool TextRange::Contain(TextRange& range)
{
	return this->StartOffset <= range.StartOffset && this->EndOffset >= range.EndOffset;
}

bool TextRange::Between(TextRange& leftRange, TextRange& rightRange)
{
	return this->StartOffset > leftRange.EndOffset && this->EndOffset < rightRange.StartOffset;
}
