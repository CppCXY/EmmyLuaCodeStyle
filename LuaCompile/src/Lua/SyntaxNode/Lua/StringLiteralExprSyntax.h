#pragma once

#include "ExprSyntax.h"


class StringLiteralExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::StringLiteralExpression;
    }

    explicit StringLiteralExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

//    std::string_view GetContent(const LuaSyntaxTree& t) const;
};
