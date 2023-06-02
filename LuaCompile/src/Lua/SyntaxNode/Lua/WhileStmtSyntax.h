#pragma once

#include "BodySyntax.h"
#include "ExprSyntax.h"

class WhileStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::WhileStatement;
    }

    explicit WhileStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    ExprSyntax GetConditionExpr(const LuaSyntaxTree &t) const;

    BodySyntax GetBody(const LuaSyntaxTree &t) const;
};
