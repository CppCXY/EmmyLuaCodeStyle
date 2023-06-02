#include "LuaCompile/Lua/Kind/LuaSyntaxMultiKind.h"

bool detail::multi_match::Match(LuaSyntaxMultiKind k, LuaSyntaxNodeKind syntaxNodeKind) {
    switch (k) {
        case LuaSyntaxMultiKind::Expression: {
            return ExpressionMatch(syntaxNodeKind);
        }
        case LuaSyntaxMultiKind::Statement: {
            return StatementMatch(syntaxNodeKind);
        }
        case LuaSyntaxMultiKind::Type: {
            return TypeMatch(syntaxNodeKind);
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
        case LuaSyntaxNodeKind::ForNumberStatement:
        case LuaSyntaxNodeKind::ForRangeStatement:
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

bool detail::multi_match::TypeMatch(LuaSyntaxNodeKind k) {
    switch (k) {
        case LuaSyntaxNodeKind::FunctionType:
        case LuaSyntaxNodeKind::IdType:
        case LuaSyntaxNodeKind::ArrayType:
        case LuaSyntaxNodeKind::TableType:
        case LuaSyntaxNodeKind::ParType:
        case LuaSyntaxNodeKind::StringType:
        case LuaSyntaxNodeKind::UnionType:
        case LuaSyntaxNodeKind::GenericType: {
            return true;
        }
        default: {
            return false;
        }
    }
}

bool detail::multi_match::DocTagMatch(LuaSyntaxNodeKind k) {
    switch (k) {
        case LuaSyntaxNodeKind::DocClass:
        case LuaSyntaxNodeKind::DocInterface:
        case LuaSyntaxNodeKind::DocEnum:
        case LuaSyntaxNodeKind::DocAlias:
        case LuaSyntaxNodeKind::DocGeneric:
        case LuaSyntaxNodeKind::DocSee:

        case LuaSyntaxNodeKind::DocSince:
        case LuaSyntaxNodeKind::DocType:
        case LuaSyntaxNodeKind::DocParam:
        case LuaSyntaxNodeKind::DocReturn:
        case LuaSyntaxNodeKind::DocField:

        case LuaSyntaxNodeKind::DocPublic:
        case LuaSyntaxNodeKind::DocProtected:
        case LuaSyntaxNodeKind::DocPrivate:
        case LuaSyntaxNodeKind::DocOverride:

        case LuaSyntaxNodeKind::DocOverload:
        case LuaSyntaxNodeKind::DocDiagnostic: {
            return true;
        }
        default: {
            return false;
        }
    }
}
