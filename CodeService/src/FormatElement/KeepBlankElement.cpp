#include "CodeService/FormatElement/KeepBlankElement.h"

KeepBlankElement::KeepBlankElement(TextRange range, int blank)
	: FormatElement(range),
	  Blank(blank)
{
}

FormatElementType KeepBlankElement::GetType()
{
	return FormatElementType::KeepBlankElement;
}
