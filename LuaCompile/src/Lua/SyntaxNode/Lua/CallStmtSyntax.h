#pragma once

#include "StmtSyntax.h"
#include "SuffixedExprSyntax.h"

class CallStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::CallStatement;
    }

    explicit CallStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    SuffixedExprSyntax GetExpr(const LuaSyntaxTree& t) const;
};
