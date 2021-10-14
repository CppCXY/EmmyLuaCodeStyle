#include "CodeService/FormatElement/ExpressionElement.h"

#include "CodeService/FormatElement/KeepBlankElement.h"
#include "CodeService/FormatElement/TextElement.h"

ExpressionElement::ExpressionElement()
	: FormatElement(TextRange(0, 0))
{
}

FormatElementType ExpressionElement::GetType()
{
	return FormatElementType::ExpressionElement;
}

void ExpressionElement::AddChild(std::shared_ptr<FormatElement> child)
{
	_children.push_back(child);
	return FormatElement::AddChild(child);
}

void ExpressionElement::AddChild(std::shared_ptr<LuaAstNode> node)
{
	_children.push_back(std::make_shared<TextElement>(node->GetText(), node->GetTextRange()));
	return FormatElement::AddChild(node);
}

void ExpressionElement::KeepBlank(int blank)
{
	_children.push_back(
		std::make_shared<KeepBlankElement>(TextRange(_textRange.EndOffset, _textRange.EndOffset), blank));
}

void ExpressionElement::Serialize(FormatContext& ctx)
{
	for (int index = 0; index != _children.size(); index++)
	{
		auto element = _children[index];
		switch (element->GetType())
		{
		case FormatElementType::KeepBlankElement:
			{
				auto keepBlankElement = std::dynamic_pointer_cast<KeepBlankElement>(element);
				int blank = keepBlankElement->Blank;
				if (blank < 0)
				{
					blank = 0;
				}

				ctx.PrintBlank(1);
			}
		default:
			{
				element->Serialize(ctx);
			}
		}
	}
}
