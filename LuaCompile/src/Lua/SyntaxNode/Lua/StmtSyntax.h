#pragma once

#include "LuaBaseSyntax.h"

class StmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return detail::multi_match::StatementMatch(kind);
    }

    explicit StmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
