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

//enum class AlignCallArgs {
//    None,
//    Normal,
////    OnlyAfterMoreIndentionStatement,
////    OnlyNotExistCrossExpression,
//};

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

enum class LineSpaceType {
    Fixed,
    Keep,
    Max,
    Min
};

struct LineSpace {
    explicit LineSpace(std::size_t space = 1)
            : Type(LineSpaceType::Fixed), Space(space) {}

    explicit LineSpace(LineSpaceType type, std::size_t space = 0)
            : Type(type), Space(space) {}

    LineSpaceType Type;
    std::size_t Space;
};

enum class FunctionSingleArgSpace {
    None,
    Always, // true
    OnlyTable,
    OnlyString
};
