#include "CodeService/FormatElement/KeepElement.h"
#include "Util/format.h"

KeepElement::KeepElement(int keepBlank, bool hasLinebreak, bool allowContinueIndent)
	: _keepBlank(keepBlank),
	  _hasLinebreak(hasLinebreak),
	  _allowContinueIndent(allowContinueIndent)
{
}

FormatElementType KeepElement::GetType()
{
	return FormatElementType::KeepElement;
}

void KeepElement::Serialize(SerializeContext& ctx, ChildIterator selfIt,
                            FormatElement& parent)
{
	const int lastElementLine = GetLastValidLine(ctx, selfIt, parent);
	const int nextElementLine = GetNextValidLine(ctx, selfIt, parent);

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

void KeepElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt,
                            FormatElement& parent)
{
	if (selfIt == parent.GetChildren().begin())
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

	if (nextElementLine == lastElementLine)
	{
		if (nextOffset - lastOffset - 1 != _keepBlank)
		{
			ctx.PushDiagnosis(format(LText("here need keep {} space"), _keepBlank), TextRange(lastOffset, nextOffset), DiagnosisType::Blank);
		}
	}
	else
	{
		ctx.SetCharacterCount(0);
	}
}
