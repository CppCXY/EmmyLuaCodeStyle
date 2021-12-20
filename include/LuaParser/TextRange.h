#pragma once

#include <cstdlib>

class TextRange
{
public:
	explicit TextRange(int startOffset = 0, int endOffset = -1);

	bool IsEmpty() const;

	bool Contain(TextRange& range) const;

	bool Between(TextRange& leftRange, TextRange& rightRange) const;

	int StartOffset;
	int EndOffset;
};
