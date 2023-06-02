#pragma once

#include "StmtSyntax.h"

class BreakStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::BreakStatement;
    }

    explicit BreakStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    StmtSyntax GetLoopStmt(const LuaSyntaxTree &t) const;
};
