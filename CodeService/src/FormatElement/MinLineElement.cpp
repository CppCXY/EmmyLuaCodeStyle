#include "CodeService/FormatElement/MinLineElement.h"

MinLineElement::MinLineElement(int line)
	: Line(line)
{
}

FormatElementType MinLineElement::GetType()
{
	return FormatElementType::MinLineElement;
}
