#pragma once

#
#include "FormatElement.h"

class KeepBlankElement : public FormatElement
{
public:
	KeepBlankElement(int blank);

	FormatElementType GetType() override;

	int Blank;
};
