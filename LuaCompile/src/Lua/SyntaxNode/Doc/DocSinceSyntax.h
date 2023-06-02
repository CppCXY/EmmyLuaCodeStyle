#pragma once

#include "DocBaseSyntax.h"

class DocSinceSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocSince;
    }

    explicit DocSinceSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetContent(const LuaSyntaxTree& t) const;
};
