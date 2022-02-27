#include "CodeService/FormatElement/StatementElement.h"

FormatElementType StatementElement::GetType()
{
	return FormatElementType::StatementElement;
}

void StatementElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.PrintLine(1);
}

void StatementElement::Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	FormatElement::Diagnose(ctx, selfIt, parent);
	ctx.SetCharacterCount(0);
}
