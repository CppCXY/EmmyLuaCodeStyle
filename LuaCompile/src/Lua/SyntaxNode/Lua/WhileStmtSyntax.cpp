#include "WhileStmtSyntax.h"

WhileStmtSyntax::WhileStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

ExprSyntax WhileStmtSyntax::GetConditionExpr(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(t);
}

BodySyntax WhileStmtSyntax::GetBody(const LuaSyntaxTree &t) const {
    return GetMember<BodySyntax>(t);
}
