#pragma once

#include "FormatElement.h"

class AlignmentLayoutElement : public FormatElement
{
public:
	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;

private:
	void alignmentSerialize(FormatContext& ctx, int position, FormatElement* parent, int eqPosition);
	void normalSerialize(FormatContext& ctx, int position, FormatElement* parent);
};