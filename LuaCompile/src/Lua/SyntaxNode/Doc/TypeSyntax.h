#pragma once

#include "DocBaseSyntax.h"

class TypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind);

    explicit TypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
