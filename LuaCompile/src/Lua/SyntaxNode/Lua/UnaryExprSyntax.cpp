#include "UnaryExprSyntax.h"

UnaryExprSyntax::UnaryExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

UnOpr UnaryExprSyntax::GetUnaryOp(const LuaSyntaxTree &t) const {
    auto op = _node.GetChildToken(
            [](auto kind) -> bool {
                return detail::lua_opr::GetUnaryOperator(kind) != UnOpr::OPR_NOUNOPR;
            },
            t);
    return detail::lua_opr::GetUnaryOperator(op.GetTokenKind(t));
}

ExprSyntax UnaryExprSyntax::GetInnerExpr(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(t);
}
