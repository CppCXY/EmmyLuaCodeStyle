#include "IndexExprSyntax.h"

IndexExprSyntax::IndexExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

ExprSyntax IndexExprSyntax::GetExpr(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(t);
}

bool IndexExprSyntax::IsDot(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_DOT, t).IsToken(t);
}

bool IndexExprSyntax::IsColon(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_COLON, t).IsToken(t);
}

bool IndexExprSyntax::IsRect(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_LBRACKET, t).IsToken(t);
}
