#include "CodeService/FormatElement/MaxLineElement.h"
#include "Util/format.h"

MaxLineElement::MaxLineElement(int line)
	: _line(line)
{
}

FormatElementType MaxLineElement::GetType()
{
	return FormatElementType::MaxLineElement;
}

void MaxLineElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	auto maxLine = 0;
	if (_line > 0)
	{
		maxLine = _line;
	}

	const int lastElementLine = GetLastValidLine(ctx, selfIt, parent);
	const int nextElementLine = GetNextValidLine(ctx, selfIt, parent);

	if (nextElementLine == -1)
	{
		return;
	}

	int line = nextElementLine - lastElementLine - 1;

	if (line > maxLine)
	{
		line = maxLine;
	}

	ctx.PrintLine(line);
}

void MaxLineElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
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
	const int nextElementLine = ctx.GetLine(nextOffset);

	if (_line < (nextElementLine - lastElementLine - 1))
	{
		auto character = ctx.GetColumn(nextOffset);
		ctx.PushDiagnosis(Util::format(LText("the maximum blank lines is {}, but found {}"), _line, (nextElementLine - lastElementLine - 1)),
			LuaDiagnosisPosition(nextElementLine, character),
			LuaDiagnosisPosition(nextElementLine + 1, 0), DiagnosisType::StatementLineSpace);
	}
	ctx.SetCharacterCount(0);
}
