#include "CodeService/FormatElement/IndentElement.h"

IndentElement::IndentElement(int specialIndent)
	: FormatElement(TextRange(0, 0)),
	  _specialIndent(specialIndent)
{
}

FormatElementType IndentElement::GetType()
{
	return FormatElementType::IndentElement;
}

void IndentElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	ctx.AddIndent(_specialIndent);

	FormatElement::Serialize(ctx, position, parent);

	ctx.RecoverIndent();
}
