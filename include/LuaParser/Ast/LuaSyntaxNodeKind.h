#pragma once
#include <string_view>

enum class LuaSyntaxNodeKind {
    None = 0,

    File,

    Block,

    EmptyStatement,

    LocalStatement,

    LocalFunctionStatement,

    IfStatement,

    WhileStatement,

    DoStatement,

    ForStatement,

    RepeatStatement,

    FunctionStatement,

    LabelStatement,

    BreakStatement,

    ReturnStatement,

    GotoStatement,

    ExpressionStatement,

    AssignStatement,

    SuffixedExpression,

    ParExpression,

    LiteralExpression,

    StringLiteralExpression,

    ClosureExpression,

    UnaryExpression,

    BinaryExpression,

    TableExpression,

    CallExpression,

    IndexExpression,

    NameExpression,

    FunctionNameExpression,

    VarList,

    TableFieldList,

    TableField,

    TableFieldSep,

    FunctionBody,

    ParamList,

    NameDefList,

    Attribute,

    ExpressionList,

    ForNumber,

    ForList,

    ForBody,

    Error,

    Comment,

    ShortComment,

    LongComment,

    ShebangComment,

    DocTagFormat,
};

namespace detail {
    namespace debug {
        std::string_view GetSyntaxKindDebugName(LuaSyntaxNodeKind kind);
    }
}