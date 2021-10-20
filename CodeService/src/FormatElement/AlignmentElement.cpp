#include "CodeService/FormatElement/AlignmentElement.h"
#include "CodeService/FormatElement/StatementElement.h"

AlignmentElement::AlignmentElement(int alignmentPosition)
	: _alignmentPosition(alignmentPosition)
{
}

FormatElementType AlignmentElement::GetType()
{
	return FormatElementType::AlignmentElement;
}

void AlignmentElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	int blank = _alignmentPosition - ctx.GetCharacterCount();
	if (blank > 0)
	{
		ctx.PrintBlank(blank);
	}
}
