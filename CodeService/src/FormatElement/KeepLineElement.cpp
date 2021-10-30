#include "CodeService/FormatElement/KeepLineElement.h"


KeepLineElement::KeepLineElement(int line)
	: FormatElement(),
	  _line(line)
{
}

FormatElementType KeepLineElement::GetType()
{
	return FormatElementType::KeepLineElement;
}

void KeepLineElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	int line = _line;
	if (_line == -1)
	{
		int lastElementLine = getLastValidLine(ctx, position, parent);
		int nextElementLine = getNextValidLine(ctx, position, parent);

		if(nextElementLine == -1)
		{
			return;
		}

		line = nextElementLine - lastElementLine - 1;
		if (line < 0)
		{
			line = 0;
		}
	}

	ctx.PrintLine(line);
}

int KeepLineElement::getLastValidLine(FormatContext& ctx, int position, FormatElement& parent)
{
	auto& siblings = parent.GetChildren();
	for (int index = position - 1; index >= 0; index--)
	{
		if(siblings[index]->HasValidTextRange())
		{
			return ctx.GetLine(siblings[index]->GetTextRange().EndOffset);
		}
	}

	// 那么一定是往上找不到有效范围元素
	return ctx.GetLine(parent.GetTextRange().StartOffset);
}

int KeepLineElement::getNextValidLine(FormatContext& ctx, int position, FormatElement& parent)
{
	auto& siblings = parent.GetChildren();
	for (std::size_t index = position + 1; index < siblings.size(); index++)
	{
		if (siblings[index]->HasValidTextRange())
		{
			return ctx.GetLine(siblings[index]->GetTextRange().StartOffset);
		}
	}

	return -1;
}
