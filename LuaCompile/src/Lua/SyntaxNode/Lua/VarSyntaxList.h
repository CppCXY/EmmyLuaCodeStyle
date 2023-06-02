#pragma once

#include "ExprSyntax.h"
#include "LuaBaseSyntax.h"

class VarSyntaxList : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::VarList;
    }

    explicit VarSyntaxList(LuaNodeOrToken n);

    std::vector<ExprSyntax> GetVars(const LuaSyntaxTree &t) const;
};
