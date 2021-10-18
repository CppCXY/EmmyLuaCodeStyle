#include "CodeService/FormatElement/IndentElement.h"

IndentElement::IndentElement()
	: FormatElement(TextRange(0,0))
{
}

FormatElementType IndentElement::GetType()
{
	return FormatElementType::IndentElement;
}

void IndentElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	ctx.AddIndent();

	FormatElement::Serialize(ctx, position, parent);

	ctx.RecoverIndent();
}
