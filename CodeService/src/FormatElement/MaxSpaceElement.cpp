#include "CodeService/FormatElement/MaxSpaceElement.h"
#include "Util/format.h"

MaxSpaceElement::MaxSpaceElement(int space)
	: _space(space)
{
}

FormatElementType MaxSpaceElement::GetType()
{
	return FormatElementType::MaxSpaceElement;
}

void MaxSpaceElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	auto maxSpace = 0;
	if (_space > 0)
	{
		maxSpace = _space;
	}

	const int lastOffset = GetLastValidOffset(selfIt, parent);
	const int nextOffset = GetNextValidOffset(selfIt, parent);

	if (lastOffset == -1 || nextOffset == -1)
	{
		return;
	}

	int space = nextOffset - lastOffset - 1;

	if (space > maxSpace)
	{
		space = maxSpace;
	}

	ctx.PrintBlank(space);
}

void MaxSpaceElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	auto maxSpace = 0;
	if (_space > 0)
	{
		maxSpace = _space;
	}

	const int lastOffset = GetLastValidOffset(selfIt, parent);
	const int nextOffset = GetNextValidOffset(selfIt, parent);

	if (lastOffset == -1 || nextOffset == -1)
	{
		return;
	}

	int space = nextOffset - lastOffset - 1;

	if (space > maxSpace)
	{
		ctx.PushDiagnosis(Util::format(LText("the maximum whitespace length is {}"), maxSpace),
		                  TextRange(lastOffset, nextOffset), DiagnosisType::Blank);
	}
}
