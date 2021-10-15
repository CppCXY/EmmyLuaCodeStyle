#pragma once

#include "FormatElement.h"

class LineElement : public FormatElement
{
public:
	LineElement(TextRange range);

	FormatElementType GetType() override;
};
