#include "CodeService/FormatElement/MinLineElement.h"

MinLineElement::MinLineElement(TextRange range, int line)
	: FormatElement(range),
	  Line(line)
{
}

FormatElementType MinLineElement::GetType()
{
	return FormatElementType::MinLineElement;
}
