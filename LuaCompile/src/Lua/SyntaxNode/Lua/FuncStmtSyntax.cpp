#include "FuncStmtSyntax.h"

FuncStmtSyntax::FuncStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

FuncNameExprSyntax FuncStmtSyntax::GetFuncNameExpr(const LuaSyntaxTree &t) const {
    return GetMember<FuncNameExprSyntax>(t);
}

ParamSyntaxList FuncStmtSyntax::GetParamList(const LuaSyntaxTree &t) const {
    return GetMember<ParamSyntaxList>(t);
}

BodySyntax FuncStmtSyntax::GetBody(const LuaSyntaxTree &t) const {
    return GetMember<BodySyntax>(t);
}
