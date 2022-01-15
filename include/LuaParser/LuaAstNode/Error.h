#pragma once

#include "LuaAstNode.h"

class Error : public LuaAstNode 
{
public:
    LUA_AST(Error);
}