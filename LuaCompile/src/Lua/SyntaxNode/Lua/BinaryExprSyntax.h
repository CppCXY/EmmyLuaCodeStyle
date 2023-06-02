#pragma once

#include "ExprSyntax.h"
#include "Lua/Define/LuaOperatorType.h"

class BinaryExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::BinaryExpression;
    }

    explicit BinaryExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    BinOpr GetBinaryOp(const LuaSyntaxTree &t) const;

    ExprSyntax GetLeftExpr(const LuaSyntaxTree &t) const;

    ExprSyntax GetRightExpr(const LuaSyntaxTree &t) const;
};
