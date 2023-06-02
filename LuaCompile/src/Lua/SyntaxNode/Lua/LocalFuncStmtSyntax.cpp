#include "LocalFuncStmtSyntax.h"

LocalFuncStmtSyntax::LocalFuncStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::string_view LocalFuncStmtSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

ParamSyntaxList LocalFuncStmtSyntax::GetParamList(const LuaSyntaxTree &t) const {
    return GetMember<ParamSyntaxList>(t);
}

BodySyntax LocalFuncStmtSyntax::GetBody(const LuaSyntaxTree &t) const {
    return GetMember<BodySyntax>(t);
}
