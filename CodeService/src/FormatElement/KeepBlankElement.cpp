#include "CodeService/FormatElement/KeepBlankElement.h"
#include "Util/format.h"

KeepBlankElement::KeepBlankElement(int blank)
	: FormatElement(),
	  _blank(blank)
{
}

FormatElementType KeepBlankElement::GetType()
{
	return FormatElementType::KeepBlankElement;
}

void KeepBlankElement::Serialize(FormatContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	int nextOffset = GetNextValidOffset(selfIt, parent);
	if (nextOffset != -1)
	{
		ctx.PrintBlank(_blank);
	}
}

void KeepBlankElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	const int lastOffset = GetLastValidOffset(selfIt, parent);
	const int nextOffset = GetNextValidOffset(selfIt, parent);

	if (nextOffset == -1)
	{
		return;
	}

	const int lastElementLine = ctx.GetLine(lastOffset);
	const int nextElementLine = ctx.GetLine(nextOffset);

	if (nextElementLine == lastElementLine)
	{
		if (nextOffset - lastOffset - 1 != _blank)
		{
			ctx.PushDiagnosis(format(LText("here need keep {} space"), _blank), TextRange(lastOffset, nextOffset));
		}
	}
}
