#pragma once

#include "FormatElement.h"

class AlignmentElement : public FormatElement
{
public:
	AlignmentElement(int alignmentPosition);
	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;

private:
	int _alignmentPosition;
};
