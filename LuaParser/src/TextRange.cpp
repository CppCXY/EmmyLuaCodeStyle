#include "LuaParser/TextRange.h"

TextRange::TextRange(int startOffset, int endOffset)
	: StartOffset(startOffset),
	  EndOffset(endOffset)
{
}

bool TextRange::IsEmpty() const
{
	return StartOffset == 0 && EndOffset == -1;
}

bool TextRange::Contain(TextRange& range) const
{
	return this->StartOffset <= range.StartOffset && this->EndOffset >= range.EndOffset;
}

bool TextRange::Between(TextRange& leftRange, TextRange& rightRange) const
{
	return this->StartOffset > leftRange.EndOffset && this->EndOffset < rightRange.StartOffset;
}
