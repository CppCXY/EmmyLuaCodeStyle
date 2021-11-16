#include "CodeService/FormatElement/AlignmentElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "Util/format.h"

AlignmentElement::AlignmentElement(int alignmentPosition)
	: _alignmentPosition(alignmentPosition)
{
}

FormatElementType AlignmentElement::GetType()
{
	return FormatElementType::AlignmentElement;
}

void AlignmentElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	int blank = _alignmentPosition - static_cast<int>(ctx.GetCharacterCount());
	if (blank > 0)
	{
		ctx.PrintBlank(blank);
	}
}

void AlignmentElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	int nextOffset = getNextValidOffset(position, parent);
	if (nextOffset == -1)
	{
		return;
	}

	auto character = ctx.GetColumn(nextOffset);
	if (character != _alignmentPosition)
	{
		ctx.PushDiagnosis(format(LText("'=' should align to character {}"), _alignmentPosition),
		                  TextRange(nextOffset, nextOffset + 1));
	}
}
