#pragma once

#include "TypeSyntax.h"

class UnionTypeSyntax : public TypeSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::UnionType;
    }

    explicit UnionTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<TypeSyntax> GetUnionTypes(const LuaSyntaxTree& t) const;
};
