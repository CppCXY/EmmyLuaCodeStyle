#pragma once

#include "BodySyntax.h"
#include "ParamSyntaxList.h"
#include "StmtSyntax.h"

class LocalFuncStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::LocalFunctionStatement;
    }

    explicit LocalFuncStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree &t) const;

    ParamSyntaxList GetParamList(const LuaSyntaxTree &t) const;

    BodySyntax GetBody(const LuaSyntaxTree &t) const;
};
