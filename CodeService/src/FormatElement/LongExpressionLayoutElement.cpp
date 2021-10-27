#include "CodeService/FormatElement/LongExpressionLayoutElement.h"

FormatElementType LongExpressionLayoutElement::GetType()
{
	return FormatElementType::KeepLayoutElement;
}

void LongExpressionLayoutElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	bool hasIndent = false;
	for (std::size_t i = 0; i != _children.size(); i++)
	{
		auto child = _children[i];

		child->Serialize(ctx, i, this);

		if(child->GetType() == FormatElementType::KeepElement)
		{
			if(ctx.GetCharacterCount() == 0 && !hasIndent)
			{
				hasIndent = true;
				ctx.AddIndent();
			}
		}
	}

	if (hasIndent) {
		ctx.RecoverIndent();
	}
}
