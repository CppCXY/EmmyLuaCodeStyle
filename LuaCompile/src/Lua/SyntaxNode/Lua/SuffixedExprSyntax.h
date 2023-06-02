#pragma once

#include "ExprSyntax.h"

class SuffixedExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::SuffixedExpression;
    }

    explicit SuffixedExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<ExprSyntax> GetExprs(const LuaSyntaxTree &t) const;
};
