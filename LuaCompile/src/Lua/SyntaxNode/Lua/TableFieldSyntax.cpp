#include "TableFieldSyntax.h"

TableFieldSyntax::TableFieldSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

ExprSyntax TableFieldSyntax::GetIndexKey(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(t);
}

std::string_view TableFieldSyntax::GetStringKey(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

ExprSyntax TableFieldSyntax::GetValueExpr(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(1, t);
}
