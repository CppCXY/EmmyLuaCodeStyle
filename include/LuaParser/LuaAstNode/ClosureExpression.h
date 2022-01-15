#pragma once

#include "LuaAstNode.h"

class ClosureExpression : public LuaAstNode 
{
public:
    LUA_AST(ClosureExpression);
}