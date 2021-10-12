#include "CodeService/FormatElement/KeepLineElement.h"

KeepLineElement::KeepLineElement(int line)
	: Line(line)
{
}

FormatElementType KeepLineElement::GetType()
{
	return FormatElementType::KeepLineElement;
}
