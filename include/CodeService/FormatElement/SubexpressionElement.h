#pragma once

#include "FormatElement.h"

class SubExpressionElement : public FormatElement
{
public:
	FormatElementType GetType() override;

	// void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;
};