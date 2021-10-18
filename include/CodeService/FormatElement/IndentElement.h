#pragma once

#include "FormatElement.h"

class IndentElement : public FormatElement
{
public:
	IndentElement();

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;

};
