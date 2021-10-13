#pragma once

#include "LuaParser/LuaAstNode.h"
#include "FormatElementType.h"

class FormatElement
{
public:
	FormatElement();
	virtual ~FormatElement();

	virtual FormatElementType GetType() = 0;

	virtual void AddChild(std::shared_ptr<LuaAstNode> node) {}

	virtual void AddChild(std::shared_ptr<FormatElement> child) {}

	virtual void Serialize() {}
};
