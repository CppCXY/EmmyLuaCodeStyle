#pragma once

#include "ExprSyntax.h"

class LiteralExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::LiteralExpression;
    }

    explicit LiteralExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    
};
