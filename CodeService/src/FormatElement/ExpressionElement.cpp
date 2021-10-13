#include "CodeService/FormatElement/ExpressionElement.h"

#include "CodeService/FormatElement/KeepBlankElement.h"
#include "CodeService/FormatElement/TextElement.h"

ExpressionElement::ExpressionElement()
{
}

FormatElementType ExpressionElement::GetType()
{
	return FormatElementType::ExpressionElement;
}

void ExpressionElement::AddChild(std::shared_ptr<FormatElement> child)
{
	_children.push_back(child);
}

void ExpressionElement::AddChild(std::shared_ptr<LuaAstNode> node)
{
	_children.push_back(std::make_shared<TextElement>(node->GetText()));
}

void ExpressionElement::KeepBlank(int blank)
{
	_children.push_back(std::make_shared<KeepBlankElement>(blank));
}
