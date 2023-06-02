#include "NameExprSyntax.h"

NameExprSyntax::NameExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::string_view NameExprSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
