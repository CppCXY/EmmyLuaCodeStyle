#include "CodeService/FormatElement/LineElement.h"

LineElement::LineElement()
	: FormatElement(TextRange())
{
}


FormatElementType LineElement::GetType()
{
	return FormatElementType::LineElement;
}

void LineElement::Serialize(FormatContext& ctx, int position,FormatElement* parent)
{
	ctx.PrintLine(1);
}
