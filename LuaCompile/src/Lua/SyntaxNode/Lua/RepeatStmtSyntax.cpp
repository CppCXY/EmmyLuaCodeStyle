#include "RepeatStmtSyntax.h"

RepeatStmtSyntax::RepeatStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

BodySyntax RepeatStmtSyntax::GetBody(const LuaSyntaxTree &t) const {
    return GetMember<BodySyntax>(t);
}

ExprSyntax RepeatStmtSyntax::GetConditionExpr(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(t);
}
