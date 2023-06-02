#pragma once

#include "StmtSyntax.h"

class GotoStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::GotoStatement;
    }

    explicit GotoStmtSyntax(LuaNodeOrToken n);

    std::string_view GetLabel(const LuaSyntaxTree &t) const;
};
