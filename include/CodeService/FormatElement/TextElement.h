#pragma once

#include "FormatElement.h"

class TextElement : public FormatElement
{
public:
	TextElement(std::string_view text);

	FormatElementType GetType() override;

	std::string_view Text;
};
 