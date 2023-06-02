#pragma once

#include "TypeSyntax.h"

class ArrayTypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ArrayType;
    }

    explicit ArrayTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    TypeSyntax GetBaseType(const LuaSyntaxTree& t) const;
};
