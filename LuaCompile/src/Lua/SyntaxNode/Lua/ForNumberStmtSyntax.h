#pragma once

#include "BodySyntax.h"
#include "ExprSyntax.h"

class ForNumberStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ForNumberStatement;
    }

    explicit ForNumberStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetItName(const LuaSyntaxTree& t);

    ExprSyntax GetItInitExpr(const LuaSyntaxTree& t);

    ExprSyntax GetItFinishExpr(const LuaSyntaxTree& t);

    ExprSyntax GetStepExpr(const LuaSyntaxTree& t);

    BodySyntax GetBody(const LuaSyntaxTree& t);
};
