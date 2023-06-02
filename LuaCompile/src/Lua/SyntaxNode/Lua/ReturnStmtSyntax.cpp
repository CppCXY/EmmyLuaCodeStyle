#include "ReturnStmtSyntax.h"

ReturnStmtSyntax::ReturnStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

ExprSyntaxList ReturnStmtSyntax::GetReturnExprList(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntaxList>(t);
}
