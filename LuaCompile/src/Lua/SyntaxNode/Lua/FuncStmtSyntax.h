#pragma once

#include "BodySyntax.h"
#include "FuncNameExprSyntax.h"
#include "ParamSyntaxList.h"
#include "StmtSyntax.h"

class FuncStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::FunctionStatement;
    }

    explicit FuncStmtSyntax(LuaNodeOrToken n);

    FuncNameExprSyntax GetFuncNameExpr(const LuaSyntaxTree& t) const;

    ParamSyntaxList GetParamList(const LuaSyntaxTree& t) const;

    BodySyntax GetBody(const LuaSyntaxTree& t) const;
};
