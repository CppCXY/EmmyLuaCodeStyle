#pragma once

#include "FormatElement.h"

class LineElement : public FormatElement
{
public:
	LineElement();

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
};
