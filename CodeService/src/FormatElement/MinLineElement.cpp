#include "CodeService/FormatElement/MinLineElement.h"

MinLineElement::MinLineElement(int line)
	: FormatElement(TextRange()),
	  _line(line)
{
}

FormatElementType MinLineElement::GetType()
{
	return FormatElementType::MinLineElement;
}

void MinLineElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	auto minLine = 0;
	if (_line > 0)
	{
		minLine = _line;
	}

	auto& siblings = parent->GetChildren();

	int firstElementLine = 0;
	int secondElementLine = 0;
	if (position == 0)
	{
		firstElementLine = ctx.GetLine(GetTextRange().StartOffset);
	}
	else
	{
		firstElementLine = ctx.GetLine(siblings[position - 1]->GetTextRange().EndOffset);
	}

	if (position == siblings.size() - 1)
	{
		// secondElementLine = ctx.GetLine(GetTextRange().EndOffset);
		// 如果是最后一个元素了就不往下打印空行了
		return;
	}
	else
	{
		secondElementLine = ctx.GetLine(siblings[position + 1]->GetTextRange().StartOffset);
	}

	int line = secondElementLine - firstElementLine - 1;
	if (line <= minLine)
	{
		line = minLine;
	}

	ctx.PrintLine(line);
}
