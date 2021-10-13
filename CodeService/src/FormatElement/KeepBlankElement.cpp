#include "CodeService/FormatElement/KeepBlankElement.h"

KeepBlankElement::KeepBlankElement(int blank)
	: Blank(blank)
{
}

FormatElementType KeepBlankElement::GetType()
{
	return FormatElementType::KeepBlankElement;
}
