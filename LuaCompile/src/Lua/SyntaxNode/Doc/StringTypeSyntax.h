#pragma once

#include "DocBaseSyntax.h"

class StringTypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::StringType;
    }

    explicit StringTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetStringText(const LuaSyntaxTree& t) const;
};
