#pragma once

#include "TypeSyntax.h"

class ParTypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ParType;
    }

    explicit ParTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    TypeSyntax GetInnerType(const LuaSyntaxTree& t) const;
};
