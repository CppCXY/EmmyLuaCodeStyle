#pragma once

#include "BodySyntax.h"
#include "ParamSyntaxList.h"

class ClosureExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ClosureExpression;
    }

    explicit ClosureExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    ParamSyntaxList GetParamList(const LuaSyntaxTree& t) const;

    BodySyntax GetBody(const LuaSyntaxTree& t) const;
};
