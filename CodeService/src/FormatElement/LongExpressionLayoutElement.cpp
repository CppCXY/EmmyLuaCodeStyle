#include "CodeService/FormatElement/LongExpressionLayoutElement.h"

LongExpressionLayoutElement::LongExpressionLayoutElement(int continuationIndent)
	: _hasContinuation(false),
	  _continuationIndent(continuationIndent)
{
}

FormatElementType LongExpressionLayoutElement::GetType()
{
	return FormatElementType::LongExpressionLayoutElement;
}

void LongExpressionLayoutElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	SerializeSubExpression(ctx, *this);
	if (_hasContinuation)
	{
		ctx.RecoverIndent();
	}
}

void LongExpressionLayoutElement::SerializeSubExpression(FormatContext& ctx, FormatElement& parent)
{
	auto& children = parent.GetChildren();
	for (int i = 0; i < static_cast<int>(children.size()); i++)
	{
		auto child = children[i];

		if (child->GetType() == FormatElementType::SubExpressionElement)
		{
			SerializeSubExpression(ctx, *child);
		}
		else
		{
			child->Serialize(ctx, i, parent);
		}
		if (child->GetType() == FormatElementType::KeepElement)
		{
			if (ctx.GetCharacterCount() == 0 && !_hasContinuation)
			{
				_hasContinuation = true;
				ctx.AddIndent(static_cast<int>(ctx.GetCurrentIndent()) + _continuationIndent);
			}
		}
	}
}
