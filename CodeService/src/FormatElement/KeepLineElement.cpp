#include "CodeService/FormatElement/KeepLineElement.h"


KeepLineElement::KeepLineElement(TextRange range, int line)
	: FormatElement(range),
	  Line(line)
{
}

FormatElementType KeepLineElement::GetType()
{
	return FormatElementType::KeepLineElement;
}
