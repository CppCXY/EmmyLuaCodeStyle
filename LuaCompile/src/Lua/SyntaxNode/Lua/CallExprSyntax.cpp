#include "CallExprSyntax.h"

CallExprSyntax::CallExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::vector<ExprSyntax> CallExprSyntax::GetArgs(const LuaSyntaxTree &t) const {
    return GetMembers<ExprSyntax>(t);
}
