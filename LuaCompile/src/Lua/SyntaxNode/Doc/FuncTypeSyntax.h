#pragma once

#include "FuncParamSyntaxList.h"
#include "TypeSyntax.h"

class FuncTypeSyntax : public TypeSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::FunctionType;
    }

    explicit FuncTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    FuncParamSyntaxList GetFuncParamTypeList(const LuaSyntaxTree& t) const;

    TypeSyntax GetReturnType(const LuaSyntaxTree& t) const;
};
