#pragma once

#include "DocBaseSyntax.h"
#include "FuncParamSyntax.h"

class FuncParamSyntaxList : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::FunctionTypeParamTypeList;
    }

    explicit FuncParamSyntaxList(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<FuncParamSyntax> GetFuncParams(const LuaSyntaxTree& t) const;
};
