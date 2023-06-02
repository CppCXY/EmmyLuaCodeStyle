#include "AssignStmtSyntax.h"

AssignStmtSyntax::AssignStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

VarSyntaxList AssignStmtSyntax::GetVarList(const LuaSyntaxTree &t) const {
    return GetMember<VarSyntaxList>(t);
}

ExprSyntaxList AssignStmtSyntax::GetExprList(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntaxList>(t);
}
