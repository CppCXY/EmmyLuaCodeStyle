#include "ExprSyntaxList.h"

ExprSyntaxList::ExprSyntaxList(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::vector<ExprSyntax> ExprSyntaxList::GetExprList(const LuaSyntaxTree &t) const {
    return GetMembers<ExprSyntax>(t);
}
