#pragma once

#include "TypeSyntax.h"

class GenericTypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::GenericType;
    }

    explicit GenericTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetBaseName(const LuaSyntaxTree &t) const;

    std::vector<TypeSyntax> GetTypeList(const LuaSyntaxTree &t) const;
};
