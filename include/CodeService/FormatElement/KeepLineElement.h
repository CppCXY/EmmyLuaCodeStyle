#pragma once

#include "FormatElement.h"

class KeepLineElement : public FormatElement
{
public:
	KeepLineElement(int line = -1);

	FormatElementType GetType() override;

	int Line;
};
