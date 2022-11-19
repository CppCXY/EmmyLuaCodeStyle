#pragma once

#include <cstdlib>

enum class NextSpaceStrategy {
    None,
    Space,
    LineBreak,
    Indent
};

enum class PrevSpaceStrategy {
    None,
    LineBreak
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

enum class LineBreakStrategy {
    Standard,
    WhenMayExceed,
};

struct LineBreakData {
    LineBreakData() : Strategy(LineBreakStrategy::Standard) {
        Data.Line = 1;
    }

    explicit LineBreakData(std::size_t line)
            : Strategy(LineBreakStrategy::Standard) {
        Data.Line = line;
    }

    LineBreakData(LineBreakStrategy strategy, std::size_t index)
            : Strategy(strategy) {
        Data.Index = index;
    }


    LineBreakStrategy Strategy;
    union BreakData {
        std::size_t Line;
        std::size_t Index;
    } Data;
};

