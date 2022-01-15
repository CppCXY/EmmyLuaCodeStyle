#pragma once

#include "LuaAstNode.h"

class EmptyStatement : public LuaAstNode 
{
public:
    LUA_AST(EmptyStatement);
}