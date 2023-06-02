#pragma once

#include "DocBaseSyntax.h"

class FieldIndexSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::FieldIndex;
    }

    explicit FieldIndexSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
