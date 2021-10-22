#include "CodeService/FormatElement/KeepElement.h"

KeepElement::KeepElement(int keepBlank)
	: KeepLineElement(0),
	  _keepBlank(keepBlank)
{
}

FormatElementType KeepElement::GetType()
{
	return FormatElementType::KeepElement;
}

void KeepElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	int lastElementLine = getLastValidLine(ctx, position, parent);
	int nextElementLine = getNextValidLine(ctx, position, parent);

	if(nextElementLine == lastElementLine)
	{
		ctx.PrintBlank(_keepBlank);
	}
	else
	{
		int line = nextElementLine - lastElementLine;
		ctx.PrintLine(line);
	}
}
