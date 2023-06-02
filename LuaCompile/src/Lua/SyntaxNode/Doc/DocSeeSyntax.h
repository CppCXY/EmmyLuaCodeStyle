#pragma once

#include "DocBaseSyntax.h"

class DocSeeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocSee;
    }

    explicit DocSeeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetContent(const LuaSyntaxTree& t) const;
};
