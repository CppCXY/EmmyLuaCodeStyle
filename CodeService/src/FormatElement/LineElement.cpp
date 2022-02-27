#include "CodeService/FormatElement/LineElement.h"

LineElement::LineElement()
	: FormatElement()
{
}


FormatElementType LineElement::GetType()
{
	return FormatElementType::LineElement;
}

void LineElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	ctx.PrintLine(1);
}

void LineElement::Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	ctx.SetCharacterCount(0);
}
