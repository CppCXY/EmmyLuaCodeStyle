#include "BinaryExprSyntax.h"

BinaryExprSyntax::BinaryExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

BinOpr BinaryExprSyntax::GetBinaryOp(const LuaSyntaxTree &t) const {
    auto op = _node.GetChildToken(
            [](auto kind) -> bool {
                return detail::lua_opr::GetBinaryOperator(kind) != BinOpr::OPR_NOBINOPR;
            },
            t);
    return detail::lua_opr::GetBinaryOperator(op.GetTokenKind(t));
}

ExprSyntax BinaryExprSyntax::GetLeftExpr(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(t);
}

ExprSyntax BinaryExprSyntax::GetRightExpr(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(1, t);
}
