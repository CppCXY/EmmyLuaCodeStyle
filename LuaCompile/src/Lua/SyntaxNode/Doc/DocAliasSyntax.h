#pragma once

#include "DocBaseSyntax.h"
#include "TypeSyntax.h"

class DocAliasSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocAlias;
    }

    explicit DocAliasSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree &t) const;

    TypeSyntax GetType(const LuaSyntaxTree &t) const;
};
