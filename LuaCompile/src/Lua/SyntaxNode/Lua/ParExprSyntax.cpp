
#include "ParExprSyntax.h"
ParExprSyntax::ParExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

ExprSyntax ParExprSyntax::GetInnerExpr(const LuaSyntaxTree &t) const {
    return ExprSyntax();
}
