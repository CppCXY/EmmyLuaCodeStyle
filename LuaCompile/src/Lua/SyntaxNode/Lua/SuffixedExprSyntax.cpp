#include "SuffixedExprSyntax.h"

SuffixedExprSyntax::SuffixedExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::vector<ExprSyntax> SuffixedExprSyntax::GetExprs(const LuaSyntaxTree &t) const {
    return GetMembers<ExprSyntax>(t);
}
