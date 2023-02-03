#pragma once

#include <cstdlib>

class TextRange
{
public:
    TextRange();

	TextRange(std::size_t startOffset, std::size_t endOffset);

	bool IsEmpty() const;

	bool Contain(TextRange& range) const;

	bool ContainOffset(std::size_t offset) const;

	bool Between(TextRange& leftRange, TextRange& rightRange) const;

	std::size_t StartOffset;
	std::size_t EndOffset;
};
