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

void KeepLineElement::Serialize(FormatContext& ctx, std::optional<FormatElement::ChildIterator> selfIt,
                                FormatElement& parent)
{
	if (!selfIt.has_value())
	{
		return;
	}

	const int lastElementLine = GetLastValidLine(ctx, selfIt.value(), parent);
	const int nextElementLine = GetNextValidLine(ctx, selfIt.value(), parent);

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

void KeepLineElement::Diagnosis(DiagnosisContext& ctx, std::optional<FormatElement::ChildIterator> selfIt,
                                FormatElement& parent)
{
	ctx.SetCharacterCount(0);

	if (!selfIt.has_value())
	{
		return;
	}

	const int lastElementOffset = GetLastValidOffset(selfIt.value(), parent);
	const int nextElementOffset = GetNextValidOffset(selfIt.value(), parent);

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
			ctx.PushDiagnosis(format(LText("here need keep {} line"), _line),
			                  TextRange(lastElementOffset, nextElementOffset));
		}
	}
}
