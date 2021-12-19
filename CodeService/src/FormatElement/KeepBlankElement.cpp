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

void KeepBlankElement::Serialize(FormatContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	if(selfIt.has_value())
	{
		int nextOffset = GetNextValidOffset(selfIt.value(), parent);
		if (nextOffset != -1)
		{
			ctx.PrintBlank(_blank);
		}
	}
}

void KeepBlankElement::Diagnosis(DiagnosisContext& ctx, std::optional<FormatElement::ChildIterator> selfIt, FormatElement& parent)
{
	if(!selfIt.has_value())
	{
		return;
	}

	const int lastOffset = GetLastValidOffset(selfIt.value(), parent);
	const int nextOffset = GetNextValidOffset(selfIt.value(), parent);

 	if (nextOffset == -1)
	{
		return;
	}

	const int lastElementLine = ctx.GetLine(lastOffset);
	const int nextElementLine = ctx.GetLine(nextOffset);

	if (nextElementLine == lastElementLine)
	{
		if (nextOffset - lastOffset - 1 != _blank)
		{
			ctx.PushDiagnosis(format(LText("here need keep {} space"), _blank), TextRange(lastOffset, nextOffset));
		}
	}
}
