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

void KeepBlankElement::Diagnosis(FormatContext& ctx, int position, FormatElement& parent)
{
	int lastElementLine = getLastValidLine(ctx, position, parent);
	int nextElementLine = getNextValidLine(ctx, position, parent);

	if (nextElementLine == -1)
	{
		return;
	}

	if (nextElementLine == lastElementLine)
	{
		int lastOffset = getLastValidOffset(ctx, position, parent);
		int nextOffset = getNextValidOffset(ctx, position, parent);
		if (nextOffset - lastOffset - 1 != _blank)
		{
			ctx.PushDiagnosis(format("must keep {} space", _blank), TextRange(lastOffset, nextOffset));
		}
	}
}
