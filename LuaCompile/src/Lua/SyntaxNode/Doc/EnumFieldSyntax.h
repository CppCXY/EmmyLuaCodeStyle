#pragma once

#include "DocBaseSyntax.h"

class EnumFieldSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::EnumField;
    }

    explicit EnumFieldSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree& t) const;


};

