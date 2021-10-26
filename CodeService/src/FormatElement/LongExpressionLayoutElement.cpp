#include "CodeService/FormatElement/LongExpressionLayoutElement.h"

FormatElementType LongExpressionLayoutElement::GetType()
{
	return FormatElementType::KeepLayoutElement;
}

void LongExpressionLayoutElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	ctx.AddIndent(-1, FormatContext::IndentStateType::ActiveIfLineBreak);
	FormatElement::Serialize(ctx, position, parent);
	ctx.RecoverIndent();
}
