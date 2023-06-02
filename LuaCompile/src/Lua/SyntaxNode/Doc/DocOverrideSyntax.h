#pragma once

#include "DocBaseSyntax.h"

class DocOverrideSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocOverride;
    }

    explicit DocOverrideSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
