#pragma once

#include "FormatElement.h"

class SubExpressionElement : public FormatElement
{
public:
	FormatElementType GetType() override;
};
