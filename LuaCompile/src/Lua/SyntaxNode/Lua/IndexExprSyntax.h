#pragma once

#include "ExprSyntax.h"

class IndexExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::IndexExpression;
    }

    explicit IndexExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    bool IsDot(const LuaSyntaxTree &t) const;

    bool IsColon(const LuaSyntaxTree &t) const;

    bool IsRect(const LuaSyntaxTree &t) const;

    ExprSyntax GetExpr(const LuaSyntaxTree &t) const;
};
