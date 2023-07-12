#pragma once

#include <cstddef>

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

enum class ContinuousAlign {
    None,
    WhenExtraSpace,
    Always
};

enum class TableSeparatorStyle {
    None,
    Comma,
    Semicolon,
    OnlyKVColon
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

enum class LineBreakReason {
    None,
    ExceedMaxLine
};

struct LineSpace {
    explicit LineSpace(std::size_t space = 1, LineBreakReason reason = LineBreakReason::None)
            : Type(LineSpaceType::Fixed), Space(space), Reason(reason) {}

    explicit LineSpace(LineSpaceType type, std::size_t space = 0)
            : Type(type), Space(space), Reason(LineBreakReason::None) {}

    LineSpaceType Type;
    std::size_t Space;
    LineBreakReason Reason;
};

enum class FunctionSingleArgSpace {
    None,
    Always, // true
    OnlyTable,
    OnlyString
};

enum class AlignArrayTable {
    None,
    Normal,
    ContainCurly
};

enum class AlignChainExpr {
    None,
    Always,
    OnlyCallStmt
};

enum class EndStmtWithSemicolon {
    Keep,
    Never,
    Always,
    SameLine
};