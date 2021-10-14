#pragma once

#include "FormatElement.h"

class TextElement : public FormatElement
{
public:
	TextElement(std::string_view text, TextRange range);

	FormatElementType GetType() override;

	// TextRange

	void Serialize(FormatContext& ctx) override;

	std::string_view Text;
};
 