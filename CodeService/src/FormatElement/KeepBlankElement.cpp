#include "CodeService/FormatElement/KeepBlankElement.h"
#include "Util/format.h"

KeepBlankElement::KeepBlankElement(int blank)
	: FormatElement(TextRange(0, 0)),
	  _blank(blank)
{
}

FormatElementType KeepBlankElement::GetType()
{
	return FormatElementType::KeepBlankElement;
}

void KeepBlankElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	ctx.PrintBlank(_blank);
}

void KeepBlankElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	int lastOffset = getLastValidOffset(position, parent);
	int nextOffset = getNextValidOffset(position, parent);

	if (nextOffset == -1)
	{
		return;
	}

	int lastElementLine = ctx.GetLine(lastOffset);
	int nextElementLine = ctx.GetLine(nextOffset);

	if (nextElementLine == lastElementLine)
	{
		if (nextOffset - lastOffset - 1 != _blank)
		{
			ctx.PushDiagnosis(format(T("here need keep {} space"), _blank), TextRange(lastOffset, nextOffset));
		}
	}
}
