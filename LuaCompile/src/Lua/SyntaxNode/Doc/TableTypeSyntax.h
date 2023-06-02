#pragma once

#include "TableTypeFieldSyntax.h"
#include "TypeSyntax.h"

class TableTypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::TableType;
    }

    explicit TableTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<TableTypeFieldSyntax> GetFields(const LuaSyntaxTree &t) const;
};
