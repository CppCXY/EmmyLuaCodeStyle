#pragma once

enum class IndentStyle {
    Tab,
    Space,
};

enum class QuoteStyle {
    None,
    Single,
    Double
};

enum class CallArgParentheses : int {
    Keep,
    Remove,
    RemoveStringOnly,
    RemoveTableOnly,
    // 应该没人用
    //UnambiguousRemoveStringOnly
};

enum class AlignCallArgs {
    None,
    Normal,
    OnlyAfterMoreIndentionStatement,
    OnlyNotExistCrossExpression,
};

enum class TableSeparatorStyle {
    None,
    Comma,
    Semicolon
};

enum class TrailingTableSeparator {
    Keep,
    Never,
    Smart,
    Always
};
