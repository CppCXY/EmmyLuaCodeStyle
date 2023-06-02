#pragma once

#include "TypeSyntax.h"

class IdTypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::IdType;
    }

    explicit IdTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree& t) const;
};
