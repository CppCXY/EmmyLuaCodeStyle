#pragma once

#include "LuaBaseSyntax.h"

class ExprSyntax : public LuaBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        switch(kind){
            case LuaSyntaxNodeKind::BinaryExpression:
            case LuaSyntaxNodeKind::SuffixedExpression:
            case LuaSyntaxNodeKind::ParExpression:
            case LuaSyntaxNodeKind::LiteralExpression:
            case LuaSyntaxNodeKind::StringLiteralExpression:
            case LuaSyntaxNodeKind::ClosureExpression:
            case LuaSyntaxNodeKind::UnaryExpression:
            case LuaSyntaxNodeKind::TableExpression:
            case LuaSyntaxNodeKind::CallExpression:
            case LuaSyntaxNodeKind::IndexExpression:
            case LuaSyntaxNodeKind::NameExpression:
            case LuaSyntaxNodeKind::FunctionNameExpression:{
                return true;
            }
            default:{
                return false;
            }
        }
    }

    explicit ExprSyntax(LuaNodeOrToken n = LuaNodeOrToken());
};
