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

void AlignmentElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	const int blank = _alignmentPosition - static_cast<int>(ctx.GetCharacterCount());
	if (blank > 0)
	{
		ctx.PrintBlank(blank);
	}
}

void AlignmentElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	const int nextOffset = GetNextValidOffset(selfIt, parent);
	if (nextOffset == -1)
	{
		return;
	}

	const auto character = ctx.GetColumn(nextOffset);
	if (character != _alignmentPosition)
	{
		ctx.PushDiagnosis(Util::format(LText("'=' should align to character {}"), _alignmentPosition),
		                  TextRange(nextOffset, nextOffset + 1), DiagnosisType::Align);
	}
}
