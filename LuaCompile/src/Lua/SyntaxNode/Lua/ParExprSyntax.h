#pragma once

#include "ExprSyntax.h"

class ParExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ParamList;
    }

    explicit ParExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    ExprSyntax GetInnerExpr(const LuaSyntaxTree& t) const;
};
