#include "CodeService/FormatElement/StatementElement.h"

#include "CodeService/FormatElement/KeepBlankElement.h"
#include "CodeService/FormatElement/TextElement.h"

FormatElementType StatementElement::GetType()
{
	return FormatElementType::StatementElement;
}
