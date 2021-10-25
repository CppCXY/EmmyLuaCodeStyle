#include "LuaParser/TextRange.h"

TextRange::TextRange(std::size_t startOffset, std::size_t endOffset)
	: StartOffset(static_cast<int>(startOffset)),
	EndOffset(static_cast<int>(endOffset))
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
