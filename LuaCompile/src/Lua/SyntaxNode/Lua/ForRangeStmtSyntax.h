#pragma once

#include "BodySyntax.h"
#include "ExprSyntax.h"

class ForRangeStmtSyntax : public LuaBaseSyntax {
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::ForRangeStatement;
    }

    explicit ForRangeStmtSyntax(LuaNodeOrToken n = LuaNodeOrToken());

//    std::vector<std::string_view> GetIterNames(const LuaSyntaxTree& t) const;
//
//    bool IsIpairs(const LuaSyntaxTree& t) const;
//
//    bool IsPairs(const LuaSyntaxTree& t) const;
//
//    ExprSyntaxList GetExprList(const LuaSyntaxTree& t) const;
//
//    BodySyntax GetBody(const LuaSyntaxTree& t) const;
};

