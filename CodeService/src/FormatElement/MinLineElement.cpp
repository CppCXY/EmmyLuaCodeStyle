#include "CodeService/FormatElement/MinLineElement.h"
#include "Util/format.h"

MinLineElement::MinLineElement(int line)
	: FormatElement(TextRange()),
	  _line(line)
{
}

FormatElementType MinLineElement::GetType()
{
	return FormatElementType::MinLineElement;
}

void MinLineElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	auto minLine = 0;
	if (_line > 0)
	{
		minLine = _line;
	}

	int lastElementLine = getLastValidLine(ctx, position, parent);
	int nextElementLine = getNextValidLine(ctx, position, parent);

	if (nextElementLine == -1)
	{
		return;
	}

	int line = nextElementLine - lastElementLine - 1;

	if (line <= minLine)
	{
		line = minLine;
	}

	ctx.PrintLine(line);
}

void MinLineElement::Diagnosis(FormatContext& ctx, int position, FormatElement& parent)
{
	if(_line <= 0)
	{
		return;
	}
	int lastElementOffset = getLastValidOffset(ctx, position, parent);
	int nextElementOffset = getNextValidOffset(ctx, position, parent);

	int lastElementLine = ctx.GetLine(lastElementOffset);
	int nextElementLine = nextElementOffset == -1 ? -1 : ctx.GetLine(nextElementOffset);

	if (nextElementLine == -1)
	{
		return;
	}

	if (_line > (nextElementLine - lastElementLine - 1))
	{
		ctx.PushDiagnosis(format("need at least {} line", _line), TextRange(lastElementOffset, nextElementOffset));
	}
}
