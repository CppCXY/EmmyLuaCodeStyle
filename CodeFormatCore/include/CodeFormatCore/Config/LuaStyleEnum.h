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
    Always

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
    Always,// true
    Keep
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
    ReplaceWithNewline,
    Always,
    SameLine,
    Never
};

enum class BreakTableList {
    Never,
    Smart,
    Lazy
};

// Controls the line break between an assignment/table-field '=' and its value.
enum class BreakAfterAssign {
    // Leave the source as it is.
    Keep,
    // Always place the value on a new line after '='.
    Always,
    // Never break after '='; collapse the value onto the '=' line.
    Never,
};

enum class SpaceAroundStyle {
    // for false and none
    None,
    // for true and always
    Always,
    // for no_space_asym
    NoSpaceAsym,
};

enum class SpaceBeforeInlineCommentStyle {
    Fixed,
    Keep,
};

struct SpaceBeforeInlineComment {
    explicit SpaceBeforeInlineComment(std::size_t space = 1, SpaceBeforeInlineCommentStyle style = SpaceBeforeInlineCommentStyle::Fixed)
        : Style(style), Space(space) {}

    SpaceBeforeInlineCommentStyle Style;
    std::size_t Space;
};