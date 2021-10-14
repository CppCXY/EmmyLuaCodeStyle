#pragma once

#include "FormatElement.h"

class MinLineElement : public FormatElement
{
public:
	MinLineElement(TextRange range, int line);

	FormatElementType GetType() override;

	int Line;
};
