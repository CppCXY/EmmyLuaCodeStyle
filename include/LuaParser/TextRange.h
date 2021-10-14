#pragma once

class TextRange
{
public:
	explicit TextRange(int startOffset = 0, int endOffset = 0);

	bool Contain(TextRange& range);

	bool Between(TextRange& leftRange, TextRange& rightRange);

	int StartOffset = 0;
	int EndOffset = 0;
};
