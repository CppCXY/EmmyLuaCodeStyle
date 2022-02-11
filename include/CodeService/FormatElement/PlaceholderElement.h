#pragma once

#include "FormatElement.h"

class PlaceholderElement : public FormatElement
{
public:
	PlaceholderElement(std::shared_ptr<FormatElement> parent, std::shared_ptr<LuaAstNode> expression);

	FormatElementType GetType() override;

	std::shared_ptr<LuaAstNode> GetRawAstNode();

	void Replace(std::shared_ptr<FormatElement> layout);
private:
	std::weak_ptr<FormatElement> _parent;
	std::shared_ptr<LuaAstNode> _expression;
};