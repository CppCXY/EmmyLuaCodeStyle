#pragma once

#include "StmtSyntax.h"

class LabelStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::LabelStatement;
    }

    explicit LabelStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetLabelName(const LuaSyntaxTree &t) const;
};
