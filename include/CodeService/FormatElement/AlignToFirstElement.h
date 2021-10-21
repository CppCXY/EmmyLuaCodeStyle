#pragma once

#include "FormatElement.h"

class AlignToFirstElement : public FormatElement
{
public:
	AlignToFirstElement(int lowestIndent);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;
private:
	int _lowestIndent;
};
