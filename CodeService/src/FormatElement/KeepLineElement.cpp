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

void KeepLineElement::Serialize(SerializeContext& ctx, ChildIterator selfIt,
                                FormatElement& parent)
{
	const int lastElementLine = GetLastValidLine(ctx, selfIt, parent);
	const int nextElementLine = GetNextValidLine(ctx, selfIt, parent);

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

void KeepLineElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt,
                                FormatElement& parent)
{
	ctx.SetCharacterCount(0);

	const int lastElementOffset = GetLastValidOffset(selfIt, parent);
	const int nextElementOffset = GetNextValidOffset(selfIt, parent);

	if (nextElementOffset == -1)
	{
		return;
	}

	if (_line != -1)
	{
		const int lastElementLine = ctx.GetLine(lastElementOffset);
		const int nextElementLine = ctx.GetLine(nextElementOffset);

		if (_line != (nextElementLine - lastElementLine - 1))
		{
			auto character = ctx.GetColumn(nextElementOffset);

			ctx.PushDiagnosis(format(LText("here need keep {} empty line"), _line),
			                  LuaDiagnosisPosition(nextElementLine, character),
			                  LuaDiagnosisPosition(nextElementLine + 1, 0), DiagnosisType::StatementLineSpace);
		}
	}
}
