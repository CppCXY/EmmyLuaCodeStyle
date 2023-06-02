#include "BodySyntax.h"

BodySyntax::BodySyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::vector<StmtSyntax> BodySyntax::GetStmts(const LuaSyntaxTree &t) {
    return GetMembers<StmtSyntax>(t);
}
