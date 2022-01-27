#include "CodeService/FormatElement/NoIndentElement.h"
#include "Util/format.h"

NoIndentElement::NoIndentElement()
	: FormatElement()
{
}

FormatElementType NoIndentElement::GetType()
{
	return FormatElementType::NoIndentElement;
}

void NoIndentElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	ctx.AddIndent(ctx.GetLastIndent());

	FormatElement::Serialize(ctx, selfIt, parent);

	ctx.RecoverIndent();
}

void NoIndentElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	ctx.AddIndent(ctx.GetLastIndent());

	GeneralIndentDiagnosis(ctx, selfIt, parent);

	ctx.RecoverIndent();
}
