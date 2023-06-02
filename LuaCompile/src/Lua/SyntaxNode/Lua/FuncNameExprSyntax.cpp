#include "FuncNameExprSyntax.h"
#include "IndexExprSyntax.h"
#include "LuaBaseSyntax.h"

FuncNameExprSyntax::FuncNameExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::string_view FuncNameExprSyntax::GetName(const LuaSyntaxTree &t) const {
    auto lastExpr = _node.GetLastChild(t);
    switch (lastExpr.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::IndexExpression:
        case LuaSyntaxNodeKind::NameExpression: {
            return lastExpr.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
        }
            // last is comment, it is rare
        default: {
            for (; !lastExpr.IsNull(t); lastExpr.ToPrev(t)) {
                if (ExprSyntax::CanCast(lastExpr.GetSyntaxKind(t))) {
                    break;
                }
            }

            return lastExpr.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
        }
    }
}

ExprSyntax FuncNameExprSyntax::GetParentExpr(const LuaSyntaxTree &t) const {
    auto skip = 1;
    for (auto lastExpr = _node.GetLastChild(t); !lastExpr.IsNull(t); lastExpr.ToPrev(t)) {
        if (ExprSyntax::CanCast(lastExpr.GetSyntaxKind(t))) {
            if (skip == 0) {
                return ExprSyntax(lastExpr);
            }

            skip--;
        }
    }
    return ExprSyntax();
}

bool FuncNameExprSyntax::IsColonDefine(const LuaSyntaxTree &t) {
    for (auto lastExpr = _node.GetLastChild(t); !lastExpr.IsNull(t); lastExpr.ToPrev(t)) {
        if (IndexExprSyntax::CanCast(lastExpr.GetSyntaxKind(t))) {
            return lastExpr.GetChildToken(LuaTokenKind::TK_COLON, t).IsToken(t);
        }
    }
    return false;
}

bool FuncNameExprSyntax::IsSimpleDefine(const LuaSyntaxTree &t) {
    for (auto lastExpr = _node.GetLastChild(t); !lastExpr.IsNull(t); lastExpr.ToPrev(t)) {
        if (IndexExprSyntax::CanCast(lastExpr.GetSyntaxKind(t))) {
            return false;
        }
    }
    return true;
}
