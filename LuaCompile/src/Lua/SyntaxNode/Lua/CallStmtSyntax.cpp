#include "CallStmtSyntax.h"

CallStmtSyntax::CallStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

SuffixedExprSyntax CallStmtSyntax::GetExpr(const LuaSyntaxTree &t) const {
    return GetMember<SuffixedExprSyntax>(t);
}
