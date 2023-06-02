#pragma once

#include "DocBaseSyntax.h"
#include "TypeSyntax.h"

class TypeSyntaxList : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::TypeList;
    }

    explicit TypeSyntaxList(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<TypeSyntax> GetTypeList(const LuaSyntaxTree &t) const;
};
