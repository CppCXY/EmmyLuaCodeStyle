#pragma once

#include "ExprSyntaxList.h"
#include "StmtSyntax.h"

class ReturnStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ReturnStatement;
    }

    explicit ReturnStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    ExprSyntaxList GetReturnExprList(const LuaSyntaxTree& t) const;
};
