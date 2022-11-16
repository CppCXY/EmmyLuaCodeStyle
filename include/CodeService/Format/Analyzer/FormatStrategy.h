#pragma once

enum class SpaceStrategy {
    None,
    Space,
    LineBreak,
    Indent
};

enum class TokenStrategy {
    Origin,
    Remove,
    StringSingleQuote,
    StringDoubleQuote,
    RemoveCommentTrailSpace
};

enum class IndentStrategy {
    None,
    Space,
    Tab,
    Mix
};

enum class LazyLineBreakStrategy {
    BreakWhenMayExceed,

};