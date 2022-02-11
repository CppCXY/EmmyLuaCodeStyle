#include "CodeService/FormatElement/PlaceholderElement.h"

PlaceholderElement::PlaceholderElement(std::shared_ptr<FormatElement> parent, std::shared_ptr<LuaAstNode> expression)
	: FormatElement(expression->GetTextRange()),
	  _parent(parent),
	  _expression(expression)
{
}

FormatElementType PlaceholderElement::GetType()
{
	return FormatElementType::PlaceholderElement;
}

std::shared_ptr<LuaAstNode> PlaceholderElement::GetRawAstNode()
{
	return _expression;
}

void PlaceholderElement::Replace(std::shared_ptr<FormatElement> layout)
{
	if (!_parent.expired())
	{
		auto parent = _parent.lock();

		auto& children = parent->GetChildren();
		for (auto& child : children)
		{
			if (child.get() == this)
			{
				child = layout;
				break;
			}
		}
	}
}
