#pragma once

#include "FormatElement.h"

class ExpressionElement : public FormatElement
{
public:
	ExpressionElement();

	FormatElementType GetType() override;
};