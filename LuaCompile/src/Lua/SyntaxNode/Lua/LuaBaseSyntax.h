#pragma once

#include "Lua/SyntaxNode/Doc/CommentSyntax.h"
#include "Lua/SyntaxNode/LuaSyntaxNode.h"

class LuaBaseSyntax : public LuaSyntaxNode {
public:
    static bool CanCast(LuaSyntaxNodeKind kind);

    explicit LuaBaseSyntax(LuaNodeOrToken node = LuaNodeOrToken());

    CommentSyntax GetComment(const LuaSyntaxTree &t) const;

    std::vector<CommentSyntax> GetComments(const LuaSyntaxTree &t) const;
};
