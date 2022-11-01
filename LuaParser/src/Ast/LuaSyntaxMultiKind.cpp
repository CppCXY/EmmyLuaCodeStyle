#include "LuaParser/Ast/LuaSyntaxMultiKind.h"

bool detail::multi_match::Match(LuaSyntaxMultiKind k, LuaSyntaxNodeKind syntaxNodeKind) {
    switch (k) {
        case LuaSyntaxMultiKind::Expression:{
            return ExpressionMatch(syntaxNodeKind);
        }
        case LuaSyntaxMultiKind::Statement: {
            return StatementMatch(syntaxNodeKind);
        }
    }
    return false;
}

bool detail::multi_match::ExpressionMatch(LuaSyntaxNodeKind k) {
    switch (k) {
        case LuaSyntaxNodeKind::SuffixedExpression:
        case LuaSyntaxNodeKind::ParExpression:
        case LuaSyntaxNodeKind::LiteralExpression:
        case LuaSyntaxNodeKind::StringLiteralExpression:
        case LuaSyntaxNodeKind::ClosureExpression:
        case LuaSyntaxNodeKind::UnaryExpression:
        case LuaSyntaxNodeKind::BinaryExpression:
        case LuaSyntaxNodeKind::TableExpression:
        case LuaSyntaxNodeKind::CallExpression:
        case LuaSyntaxNodeKind::IndexExpression:
        case LuaSyntaxNodeKind::NameExpression:
        case LuaSyntaxNodeKind::FunctionNameExpression: {
            return true;
        }
        default: {
            return false;
        }
    }
    return false;
}

bool detail::multi_match::StatementMatch(LuaSyntaxNodeKind k) {
    switch (k) {
        case LuaSyntaxNodeKind::EmptyStatement:
        case LuaSyntaxNodeKind::LocalStatement:
        case LuaSyntaxNodeKind::LocalFunctionStatement:
        case LuaSyntaxNodeKind::IfStatement:
        case LuaSyntaxNodeKind::WhileStatement:
        case LuaSyntaxNodeKind::DoStatement:
        case LuaSyntaxNodeKind::ForStatement:
        case LuaSyntaxNodeKind::RepeatStatement:
        case LuaSyntaxNodeKind::FunctionStatement:
        case LuaSyntaxNodeKind::LabelStatement:
        case LuaSyntaxNodeKind::BreakStatement:
        case LuaSyntaxNodeKind::ReturnStatement:
        case LuaSyntaxNodeKind::GotoStatement:
        case LuaSyntaxNodeKind::ExpressionStatement:
        case LuaSyntaxNodeKind::AssignStatement: {
            return true;
        }
        default: {
            return false;
        }
    }
}

