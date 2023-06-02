#pragma once

#include "ExprSyntax.h"
#include "LuaBaseSyntax.h"

class TableFieldSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::TableField;
    }

    explicit TableFieldSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    ExprSyntax GetIndexKey(const LuaSyntaxTree& t) const;

    std::string_view GetStringKey(const LuaSyntaxTree& t) const;

    ExprSyntax GetValueExpr(const LuaSyntaxTree& t) const;
};
