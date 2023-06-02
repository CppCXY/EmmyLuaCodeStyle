#include "LocalStmtSyntax.h"

LocalStmtSyntax::LocalStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

NameDefSyntaxList LocalStmtSyntax::GetNameDefList(const LuaSyntaxTree &t) const {
    return GetMember<NameDefSyntaxList>(t);
}

ExprSyntaxList LocalStmtSyntax::GetExprList(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntaxList>(t);
}
