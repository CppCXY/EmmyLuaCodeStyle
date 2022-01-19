#include "CodeService/FormatElement/StatementElement.h"

FormatElementType StatementElement::GetType()
{
	return FormatElementType::StatementElement;
}

void StatementElement::Serialize(FormatContext& ctx, ChildIterator& selfIt, FormatElement& parent)
{
	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.PrintLine(1);
}

void StatementElement::Diagnosis(DiagnosisContext& ctx, ChildIterator& selfIt, FormatElement& parent)
{
	FormatElement::Diagnosis(ctx, selfIt, parent);
	ctx.SetCharacterCount(0);
}
