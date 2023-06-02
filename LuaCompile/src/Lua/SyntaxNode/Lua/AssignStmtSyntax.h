#pragma once

#include "ExprSyntaxList.h"
#include "StmtSyntax.h"
#include "VarSyntaxList.h"

class AssignStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::AssignStatement;
    }

    explicit AssignStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    VarSyntaxList GetVarList(const LuaSyntaxTree &t) const;

    ExprSyntaxList GetExprList(const LuaSyntaxTree &t) const;
};
