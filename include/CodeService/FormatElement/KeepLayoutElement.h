#pragma once

#include "FormatElement.h"

class KeepLayoutElement : public FormatElement
{
public:
	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;
};
