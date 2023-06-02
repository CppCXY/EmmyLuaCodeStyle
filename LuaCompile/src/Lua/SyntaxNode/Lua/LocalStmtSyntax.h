#pragma once

#include "ExprSyntaxList.h"
#include "NameDefSyntaxList.h"

class LocalStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::LocalStatement;
    }

    explicit LocalStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    NameDefSyntaxList GetNameDefList(const LuaSyntaxTree &t) const;

    ExprSyntaxList GetExprList(const LuaSyntaxTree &t) const;
};
