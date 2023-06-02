#include "LuaBaseSyntax.h"

bool LuaBaseSyntax::CanCast(LuaSyntaxNodeKind kind) {
    return static_cast<int>(kind) < static_cast<int>(LuaSyntaxNodeKind::Comment) && static_cast<int>(kind) >= static_cast<int>(LuaSyntaxNodeKind::Body);
}

LuaBaseSyntax::LuaBaseSyntax(LuaNodeOrToken node) : LuaSyntaxNode(node) {
}

CommentSyntax LuaBaseSyntax::GetComment(const LuaSyntaxTree &t) const {
    return GetMember<CommentSyntax>(t);
}

std::vector<CommentSyntax> LuaBaseSyntax::GetComments(const LuaSyntaxTree &t) const {
    return GetMembers<CommentSyntax>(t);
}
