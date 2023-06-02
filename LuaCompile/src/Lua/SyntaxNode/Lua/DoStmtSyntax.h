#pragma once

#include "BodySyntax.h"

class DoStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DoStatement;
    }

    explicit DoStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    BodySyntax GetBody(const LuaSyntaxTree &t) const;
};
