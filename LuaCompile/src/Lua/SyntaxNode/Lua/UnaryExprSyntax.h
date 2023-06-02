#pragma once

#include "ExprSyntax.h"
#include "Lua/Define/LuaOperatorType.h"

class UnaryExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::UnaryExpression;
    }

    explicit UnaryExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    UnOpr GetUnaryOp(const LuaSyntaxTree& t) const;

    ExprSyntax GetInnerExpr(const LuaSyntaxTree &t) const;
};
