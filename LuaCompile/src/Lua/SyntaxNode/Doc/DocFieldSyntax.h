#pragma once

#include "DocBaseSyntax.h"
#include "FieldIndexSyntax.h"
#include "Lua/Define/AccessVisibility.h"
#include "TypeSyntax.h"

class DocFieldSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocField;
    }

    explicit DocFieldSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    AccessVisibility GetVisibility(const LuaSyntaxTree& t) const;

    bool IsNullable(const LuaSyntaxTree& t) const;

    std::string_view GetName(const LuaSyntaxTree& t) const;

    FieldIndexSyntax GetIndex(const LuaSyntaxTree& t) const;

    TypeSyntax GetType(const LuaSyntaxTree& t) const;
};
