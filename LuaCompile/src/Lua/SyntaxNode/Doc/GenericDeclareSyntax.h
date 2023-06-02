#pragma once

#include "DocBaseSyntax.h"

class GenericDeclareSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::GenericDeclare;
    }

    explicit GenericDeclareSyntax(LuaNodeOrToken n = LuaNodeOrToken());


};
