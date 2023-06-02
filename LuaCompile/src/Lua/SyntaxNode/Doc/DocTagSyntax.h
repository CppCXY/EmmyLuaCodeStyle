#pragma once

#include "DocBaseSyntax.h"

class DocTagSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return detail::multi_match::DocTagMatch(kind);
    }

    explicit DocTagSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
