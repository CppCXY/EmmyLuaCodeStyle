#pragma once

#include "DocBaseSyntax.h"

class FuncParamSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::FunctionParam;
    }

    explicit FuncParamSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree& t) const;
};
