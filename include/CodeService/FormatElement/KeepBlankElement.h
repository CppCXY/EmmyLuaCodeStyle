#pragma once

#
#include "FormatElement.h"

class KeepBlankElement : public FormatElement
{
public:
	KeepBlankElement(TextRange range, int blank);

	FormatElementType GetType() override;

	int Blank;
};
