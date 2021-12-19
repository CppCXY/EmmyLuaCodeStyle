#include "CodeService/FormatElement/KeepElement.h"
#include "Util/format.h"

KeepElement::KeepElement(int keepBlank, bool hasLinebreak)
	: _keepBlank(keepBlank),
	  _hasLinebreak(hasLinebreak)
{
}

FormatElementType KeepElement::GetType()
{
	return FormatElementType::KeepElement;
}

void KeepElement::Serialize(FormatContext& ctx, std::optional<FormatElement::ChildIterator> selfIt,
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
	// 这个条件的意思是如果上一个元素和下一个元素没有实质的换行则保持一定的空格
	if (nextElementLine == lastElementLine && ctx.GetCharacterCount() != 0)
	{
		ctx.PrintBlank(_keepBlank);
	}
	else
	{
		int line = nextElementLine - lastElementLine;
		if (_hasLinebreak)
		{
			line--;
		}
		ctx.PrintLine(line);
	}
}

void KeepElement::Diagnosis(DiagnosisContext& ctx, std::optional<FormatElement::ChildIterator> selfIt,
                            FormatElement& parent)
{
	if (!selfIt.has_value())
	{
		return;
	}

	const auto it = selfIt.value();
	if(it == parent.GetChildren().begin())
	{
		return;
	}

	const int lastOffset = GetLastValidOffset(it, parent);
	const int nextOffset = GetNextValidOffset(it, parent);

	if (nextOffset == -1)
	{
		return;
	}

	const int lastElementLine = ctx.GetLine(lastOffset);
	const int nextElementLine = ctx.GetLine(nextOffset);

	if (nextElementLine == lastElementLine)
	{
		if (nextOffset - lastOffset - 1 != _keepBlank)
		{
			ctx.PushDiagnosis(format(LText("here need keep {} space"), _keepBlank), TextRange(lastOffset, nextOffset));
		}
	}
	else
	{
		ctx.SetCharacterCount(0);
	}
}
