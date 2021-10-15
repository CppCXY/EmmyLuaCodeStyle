#include "CodeService/FormatElement/LineElement.h"

LineElement::LineElement(TextRange range)
	:FormatElement(range)
{
}

FormatElementType LineElement::GetType()
{
	return FormatElementType::LineElement;
}
