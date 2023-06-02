#pragma once

#include "BodySyntax.h"
#include "ExprSyntax.h"
#include "StmtSyntax.h"

class RepeatStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::RepeatStatement;
    }

    explicit RepeatStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    BodySyntax GetBody(const LuaSyntaxTree &t) const;

    ExprSyntax GetConditionExpr(const LuaSyntaxTree &t) const;
};
