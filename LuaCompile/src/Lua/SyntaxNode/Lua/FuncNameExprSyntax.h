#pragma once

#include "ExprSyntax.h"

class FuncNameExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::FunctionNameExpression;
    }

    explicit FuncNameExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree& t) const;

    ExprSyntax GetParentExpr(const LuaSyntaxTree& t) const;

    bool IsColonDefine(const LuaSyntaxTree& t);

    bool IsSimpleDefine(const LuaSyntaxTree& t);
};
