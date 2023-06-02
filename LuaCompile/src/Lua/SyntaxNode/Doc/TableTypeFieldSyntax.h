#pragma once

#include "DocBaseSyntax.h"

class TableTypeFieldSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::TableTypeField;
    }

    explicit TableTypeFieldSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
