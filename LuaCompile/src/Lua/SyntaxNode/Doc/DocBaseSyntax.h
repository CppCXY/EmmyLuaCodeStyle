#pragma once

#include "Lua/SyntaxNode/LuaSyntaxNode.h"

class DocBaseSyntax : public LuaSyntaxNode {
public:
    static bool CanCast(LuaSyntaxNodeKind kind);

    explicit DocBaseSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
