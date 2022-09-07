#include "LuaParser/TextRange.h"

TextRange::TextRange(int startOffset, int endOffset)
	: StartOffset(startOffset),
	  EndOffset(endOffset)
{
}

TextRange::TextRange(std::size_t startOffset, std::size_t endOffset)
	:
	StartOffset(static_cast<int>(startOffset)),
	EndOffset(static_cast<int>(endOffset))
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

bool TextRange::ContainOffset(int offset) const
{
	return this->StartOffset <= offset && offset <= this->EndOffset;
}

bool TextRange::Between(TextRange& leftRange, TextRange& rightRange) const
{
	return this->StartOffset > leftRange.EndOffset && this->EndOffset < rightRange.StartOffset;
}
