#include "ForNumberStmtSyntax.h"

ForNumberStmtSyntax::ForNumberStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::string_view ForNumberStmtSyntax::GetItName(const LuaSyntaxTree &t) {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

ExprSyntax ForNumberStmtSyntax::GetItInitExpr(const LuaSyntaxTree &t) {
    return GetMember<ExprSyntax>(t);
}

ExprSyntax ForNumberStmtSyntax::GetItFinishExpr(const LuaSyntaxTree &t) {
    return GetMember<ExprSyntax>(1, t);
}

ExprSyntax ForNumberStmtSyntax::GetStepExpr(const LuaSyntaxTree &t) {
    return GetMember<ExprSyntax>(2, t);
}

BodySyntax ForNumberStmtSyntax::GetBody(const LuaSyntaxTree &t) {
    return GetMember<BodySyntax>(t);
}
