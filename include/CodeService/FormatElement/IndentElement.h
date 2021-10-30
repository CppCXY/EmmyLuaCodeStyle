#pragma once

#include "FormatElement.h"

class IndentElement : public FormatElement
{
public:
	IndentElement(int specialIndent = -1);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
private:
	int _specialIndent;
};
