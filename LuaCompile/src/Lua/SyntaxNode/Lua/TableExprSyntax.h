#pragma once

#include "ExprSyntax.h"
#include "TableFieldSyntax.h"

class TableExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::TableExpression;
    }

    explicit TableExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<TableFieldSyntax> GetFields(const LuaSyntaxTree &t) const;
};
