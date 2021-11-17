#include "CodeService/FormatElement/StatementElement.h"

FormatElementType StatementElement::GetType()
{
	return FormatElementType::StatementElement;
}

void StatementElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	FormatElement::Serialize(ctx, position, parent);

	ctx.PrintLine(1);
}

void StatementElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	FormatElement::Diagnosis(ctx, position, parent);
	ctx.SetCharacterCount(0);
}
