#include "CodeService/FormatElement/TextElement.h"

TextElement::TextElement(std::string_view text)
	:Text(text)
{
}

FormatElementType TextElement::GetType()
{
	return FormatElementType::TextElement;
}
