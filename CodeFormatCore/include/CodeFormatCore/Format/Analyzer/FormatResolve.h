#pragma once

#include "CodeFormatCore/Format/Types.h"
#include "FormatStrategy.h"
#include <cinttypes>

class FormatResolve {
public:
    FormatResolve();

    NextSpaceStrategy GetNextSpaceStrategy() const;

    PrevSpaceStrategy GetPrevSpaceStrategy() const;

    PrevTokenStrategy GetPrevTokenStrategy() const;

    TokenStrategy GetTokenStrategy() const;

    NextTokenStrategy GetNextTokenStrategy() const;

    IndentStrategy GetIndentStrategy() const;

    void SetNextSpace(std::size_t space);

    void SetNextLineBreak(LineSpace lineSpace);

    void SetIndent(std::size_t indent = 0, IndentStrategy strategy = IndentStrategy::Relative);

    void SetAlign(std::size_t alignPos);

    void SetRelativeIndentAlign(std::size_t align);

    void SetTokenStrategy(TokenStrategy strategy);

    void SetNextTokenStrategy(NextTokenStrategy strategy);

    void SetPrevTokenStrategy(PrevTokenStrategy strategy);

    std::size_t GetNextSpace();

    LineSpace GetNextLine();

    std::size_t GetIndent() const;

    std::size_t GetAlign() const;

    void SetOriginRange(IndexRange range);

    IndexRange GetOriginRange() const;

    void Reset();

private:
    NextSpaceStrategy _nextSpaceStrategy;
    PrevSpaceStrategy _prevSpaceStrategy;
    PrevTokenStrategy _prevTokenStrategy;
    TokenStrategy _tokenStrategy;
    NextTokenStrategy _nextAddStrategy;
    IndentStrategy _indentStrategy;

    union NextData {
        NextData() : Line() {}

        std::size_t Space;
        LineSpace Line;
    } _nextSpaceData;

    union PrevData {
        std::size_t Align;
    } _prevSpaceData;

    std::size_t _indent;
    IndexRange _range;
};
