#pragma once

#include "LuaBaseSyntax.h"

class ParamSyntaxList : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ParamList;
    }

    explicit ParamSyntaxList(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<std::string_view> GetParams(const LuaSyntaxTree& t) const;
};

