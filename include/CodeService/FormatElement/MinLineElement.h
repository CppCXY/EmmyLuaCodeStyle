#pragma once

#include "FormatElement.h"

class MinLineElement: public FormatElement
{
public:
	MinLineElement(int line);

	FormatElementType GetType() override;

	int Line;
};
