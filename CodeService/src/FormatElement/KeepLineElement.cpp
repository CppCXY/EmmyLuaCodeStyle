#include "CodeService/FormatElement/KeepLineElement.h"
#include "Util/format.h"

KeepLineElement::KeepLineElement(int line)
	: FormatElement(),
	  _line(line)
{
}

FormatElementType KeepLineElement::GetType()
{
	return FormatElementType::KeepLineElement;
}

void KeepLineElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	int lastElementLine = getLastValidLine(ctx, position, parent);
	int nextElementLine = getNextValidLine(ctx, position, parent);

	if (nextElementLine == -1)
	{
		return;
	}

	int line = _line;
	if (line == -1)
	{
		line = nextElementLine - lastElementLine - 1;
		if (line < 0)
		{
			line = 0;
		}
	}

	ctx.PrintLine(line);
}

void KeepLineElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	ctx.SetCharacterCount(0);

	int lastElementOffset = getLastValidOffset(position, parent);
	int nextElementOffset = getNextValidOffset(position, parent);

	if (nextElementOffset == -1)
	{
		return;
	}

	if (_line != -1)
	{
		int lastElementLine = ctx.GetLine(lastElementOffset);
		int nextElementLine = ctx.GetLine(nextElementOffset);

		if (_line != (nextElementLine - lastElementLine - 1))
		{
			ctx.PushDiagnosis(format(LText("here need keep {} line"), _line),
			                  TextRange(lastElementOffset, nextElementOffset));
		}
	}
}
