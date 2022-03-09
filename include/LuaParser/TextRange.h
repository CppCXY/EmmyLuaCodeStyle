#pragma once

#include <cstdlib>

class TextRange
{
public:
	explicit TextRange(int startOffset = 0, int endOffset = -1);

	TextRange(std::size_t startOffset, std::size_t endOffset);

	bool IsEmpty() const;

	bool Contain(TextRange& range) const;

	bool Between(TextRange& leftRange, TextRange& rightRange) const;

	int StartOffset;
	int EndOffset;
};
