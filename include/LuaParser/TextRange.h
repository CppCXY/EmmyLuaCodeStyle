#pragma once

class TextRange
{
public:
	TextRange(int startOffset, int endOffset);

	bool Contain(TextRange& range);

	bool Between(TextRange& leftRange, TextRange& rightRange);

	int StartOffset = 0;
	int EndOffset = 0;
};
