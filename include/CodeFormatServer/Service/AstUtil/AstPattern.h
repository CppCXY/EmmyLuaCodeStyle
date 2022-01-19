#pragma once

#include "LuaParser/LuaAstNode/LuaAstNodeType.h"
#include "LuaParser/LuaAstNode/LuaAstNode.h"

class AstPattern
{
public:
	AstPattern(LuaAstNodeType baseType);

	bool Match(std::shared_ptr<LuaAstNode> node);

	AstPattern& Parent(LuaAstNodeType type);
	AstPattern& InFunction();
};