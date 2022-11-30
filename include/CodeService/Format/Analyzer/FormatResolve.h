#pragma once

#include "FormatStrategy.h"
#include <cinttypes>

class FormatResolve {
public:
    FormatResolve();

    NextSpaceStrategy GetNextSpaceStrategy() const;

    PrevSpaceStrategy GetPrevSpaceStrategy() const;

    TokenStrategy GetTokenStrategy() const;

    bool HasIndent() const;

    void SetNextSpace(std::size_t space);

    void SetNextLineBreak(std::size_t line);

    void SetIndent(std::size_t indent = 0);

    void SetAlign(std::size_t alignPos);

    void SetRelativeIndentAlign(std::size_t align);

    std::size_t GetNextSpace();

    std::size_t GetNextLine();

    std::size_t GetIndent() const;

    std::size_t GetAlign() const;

    void Reset();

private:
    NextSpaceStrategy _nextSpaceStrategy;
    PrevSpaceStrategy _prevSpaceStrategy;
    TokenStrategy _tokenStrategy;
    union NextData {
        std::size_t Space;
        std::size_t Line;
    } _nextSpaceData;

    union PrevData {
        std::size_t Align;
    } _prevSpaceData;

    bool _hasIndent;
    std::size_t _indent;
};
