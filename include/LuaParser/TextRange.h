#pragma once

#include <cstdlib>

class TextRange
{
public:
	explicit TextRange(std::size_t startOffset = 0, std::size_t endOffset = 0);

	bool Contain(TextRange& range);

	bool Between(TextRange& leftRange, TextRange& rightRange);

	int StartOffset = 0;
	int EndOffset = 0;
};
