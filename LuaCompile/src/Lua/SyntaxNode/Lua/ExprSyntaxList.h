#pragma once

#include "ExprSyntax.h"
#include "LuaBaseSyntax.h"

class ExprSyntaxList : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ExpressionList;
    }

    explicit ExprSyntaxList(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<ExprSyntax> GetExprList(const LuaSyntaxTree &t) const;
};
