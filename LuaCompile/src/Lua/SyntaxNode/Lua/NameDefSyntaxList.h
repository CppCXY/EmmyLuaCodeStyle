#pragma once

#include "LuaBaseSyntax.h"
#include "NameDefSyntax.h"

class NameDefSyntaxList : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::NameDefList;
    }

    explicit NameDefSyntaxList(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<NameDefSyntax> GetList(const LuaSyntaxTree &t) const;
};
