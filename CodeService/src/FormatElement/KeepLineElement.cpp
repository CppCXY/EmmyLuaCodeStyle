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
	int line = _line;
	if (line == -1)
	{
		int lastElementLine = getLastValidLine(ctx, position, parent);
		int nextElementLine = getNextValidLine(ctx, position, parent);

		if (nextElementLine == -1)
		{
			return;
		}

		line = nextElementLine - lastElementLine - 1;
		if (line < 0)
		{
			line = 0;
		}
	}

	ctx.PrintLine(line);
}

void KeepLineElement::Diagnosis(FormatContext& ctx, int position, FormatElement& parent)
{
	if (_line != -1)
	{
		int lastElementOffset = getLastValidOffset(ctx, position, parent);
		int nextElementOffset = getNextValidOffset(ctx, position, parent);

		int lastElementLine = ctx.GetLine(lastElementOffset);
		int nextElementLine = nextElementOffset == -1 ? -1 : ctx.GetLine(nextElementOffset);

		if (nextElementLine == -1)
		{
			return;
		}

		if (_line != (nextElementLine - lastElementLine - 1))
		{
			ctx.PushDiagnosis(format("need {} line", _line), TextRange(lastElementOffset, nextElementOffset));
		}
	}
}
