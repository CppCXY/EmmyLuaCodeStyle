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

void MinLineElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	if (_line <= 0)
	{
		return;
	}
	int lastOffset = getLastValidOffset(position, parent);
	int nextOffset = getNextValidOffset(position, parent);

	if (nextOffset == -1)
	{
		return;
	}

	int lastElementLine = ctx.GetLine(lastOffset);
	int nextElementLine = nextOffset == -1 ? -1 : ctx.GetLine(nextOffset);

	if (_line > (nextElementLine - lastElementLine - 1))
	{
		ctx.PushDiagnosis(format(LText("here need at least {} line"), _line), TextRange(lastOffset, nextOffset));
	}
	ctx.SetCharacterCount(0);
}
