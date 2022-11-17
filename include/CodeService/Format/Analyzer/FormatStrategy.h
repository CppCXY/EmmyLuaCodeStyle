#pragma once
#include <cstdlib>

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
    Standard,
    WhenLineBreak,
    WhenPrevIndent
};

struct IndentData {
    IndentStrategy Strategy = IndentStrategy::Standard;
    std::size_t Data = 0;
};

enum class LazyLineBreakStrategy {
    BreakWhenMayExceed,
};