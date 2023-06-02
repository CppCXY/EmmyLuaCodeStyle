#pragma once

#include "ExprSyntax.h"

class CallExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::CallExpression;
    }

    explicit CallExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<ExprSyntax> GetArgs(const LuaSyntaxTree& t) const;
};
