#pragma once

#include "LuaSyntaxNode.h"


class LuaBody : public LuaSyntaxNode {
public:
    bool CanCast(LuaSyntaxNodeKind kind) {
        return true;
    }

    explicit LuaBody(LuaNodeOrToken n): LuaSyntaxNode(n) {}



};

