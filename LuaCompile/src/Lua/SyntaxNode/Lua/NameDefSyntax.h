#pragma once

#include "LuaBaseSyntax.h"

class NameDefSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::NameDef;
    }

    explicit NameDefSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree &t) const;
};
