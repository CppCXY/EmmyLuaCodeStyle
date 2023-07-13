#pragma once

#include "CodeFormatCore/Config/LuaStyleEnum.h"
#include <cstdlib>
#include <vector>

enum class NextSpaceStrategy {
    None,
    Space,
    LineBreak
};

enum class PrevSpaceStrategy {
    None,
    AlignPos,
    AlignRelativeIndent,
    LineBreak
};

enum class TokenStrategy {
    Origin,
    Remove,
    StringSingleQuote,
    StringDoubleQuote,
    TableSepSemicolon,
    TableSepComma,
    OriginRange,
    StmtEndSemicolon,
    NewLineBeforeToken
};

enum class TokenAddStrategy {
    None,
    TableAddColon,
    TableAddComma,
    StmtEndSemicolon
};

enum class IndentStrategy {
    None,
    Relative,
    InvertRelative,
    Absolute,
    Same
};

enum class IndentType {
    None,
    Standard,
    InvertIndentation,
    WhenNewLine,
    WhenPrevIndent,
    WhenExceedLinebreak
};

struct IndentData {
    IndentData(IndentType type = IndentType::Standard, std::size_t size = 0)
        : Type(type), Indent(size) {}

    IndentType Type;
    std::size_t Indent;
};

enum class LineBreakStrategy {
    Standard,
    WhenMayExceed,
    NotBreak
};

struct LineBreakData {
    LineBreakData() : Strategy(LineBreakStrategy::Standard), Data() {
    }

    explicit LineBreakData(std::size_t line)
        : Strategy(LineBreakStrategy::Standard),
          Data(LineSpace(LineSpaceType::Fixed, line)) {
    }

    explicit LineBreakData(LineSpace lineSpace)
        : Strategy(LineBreakStrategy::Standard),
          Data(lineSpace) {
    }

    LineBreakData(LineBreakStrategy strategy, std::size_t index)
        : Strategy(strategy),
          Data(index) {
    }

    LineBreakStrategy Strategy;

    union BreakData {
        BreakData()
            : Line() {
        }

        explicit BreakData(LineSpace line)
            : Line(line) {
        }

        explicit BreakData(std::size_t index)
            : Index(index) {
        }

        LineSpace Line;
        std::size_t Index;
    } Data;
};

enum class AlignStrategy {
    Normal,
    AlignToEqWhenExtraSpace,
    AlignToEqAlways,
    AlignToFirst,
    AlignComment,
};

struct AlignGroup {
    AlignGroup(AlignStrategy strategy, const std::vector<std::size_t> &group)
        : Strategy(strategy), SyntaxGroup(group), Resolve(false), AlignPos(0) {}

    AlignStrategy Strategy;
    std::vector<std::size_t> SyntaxGroup;
    bool Resolve;
    std::size_t AlignPos;
};

enum class SemicolonStrategy {
    Add,
    Remove,
    InsertNewLine
};