#include "CodeService/FormatElement/KeepBlankElement.h"

KeepBlankElement::KeepBlankElement(int blank)
	: FormatElement(TextRange(0, 0)),
	  _blank(blank)
{
}

FormatElementType KeepBlankElement::GetType()
{
	return FormatElementType::KeepBlankElement;
}

void KeepBlankElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	ctx.PrintBlank(_blank);
}
