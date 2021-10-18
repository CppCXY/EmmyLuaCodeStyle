#include "CodeService/FormatElement/ExpressionElement.h"

ExpressionElement::ExpressionElement()
	: FormatElement()
{
}

FormatElementType ExpressionElement::GetType()
{
	return FormatElementType::ExpressionElement;
}
