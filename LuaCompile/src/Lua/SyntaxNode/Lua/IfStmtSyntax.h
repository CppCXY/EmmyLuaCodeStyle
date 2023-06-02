#pragma once

#include "BodySyntax.h"
#include "ExprSyntax.h"
#include "StmtSyntax.h"

class IfStmtSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::IfStatement;
    }

    explicit IfStmtSyntax(LuaNodeOrToken n);

    ExprSyntax GetIfCondition(const LuaSyntaxTree& t) const;

    BodySyntax GetIfBody(const LuaSyntaxTree& t) const;

    BodySyntax GetElseClauseBody(const LuaSyntaxTree& t) const;

    std::size_t GetElseIfNum(const LuaSyntaxTree& t) const;

    std::vector<std::pair<ExprSyntax, BodySyntax>> GetElseIfPairs(const LuaSyntaxTree& t) const;
};
