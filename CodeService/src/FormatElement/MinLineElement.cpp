#include "CodeService/FormatElement/MinLineElement.h"
#include "Util/format.h"

MinLineElement::MinLineElement(int line)
	: FormatElement(),
	  _line(line)
{
}

FormatElementType MinLineElement::GetType()
{
	return FormatElementType::MinLineElement;
}

void MinLineElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	auto minLine = 0;
	if (_line > 0)
	{
		minLine = _line;
	}

	const int lastElementLine = GetLastValidLine(ctx, selfIt, parent);
	const int nextElementLine = GetNextValidLine(ctx, selfIt, parent);

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

void MinLineElement::Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_line <= 0)
	{
		return;
	}

	const int lastOffset = GetLastValidOffset(selfIt, parent);
	const int nextOffset = GetNextValidOffset(selfIt, parent);

	if (nextOffset == -1)
	{
		return;
	}

	const int lastElementLine = ctx.GetLine(lastOffset);
	const int nextElementLine = nextOffset == -1 ? -1 : ctx.GetLine(nextOffset);

	if (_line > (nextElementLine - lastElementLine - 1))
	{
		ctx.PushDiagnosis(format(LText("here need at least {} line"), _line), TextRange(lastOffset, nextOffset));
	}
	ctx.SetCharacterCount(0);
}
