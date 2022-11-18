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
    IndentData(IndentStrategy strategy = IndentStrategy::Standard, std::size_t size = 0)
            : Strategy(strategy), Data(size) {}

    IndentStrategy Strategy;
    std::size_t Data;
};

enum class LazyLineBreakStrategy {
    BreakWhenMayExceed,
};