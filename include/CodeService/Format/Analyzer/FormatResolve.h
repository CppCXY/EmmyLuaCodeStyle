#pragma once

#include "FormatStrategy.h"
#include <cinttypes>

class FormatResolve {
public:
    FormatResolve();

    SpaceStrategy GetSpaceStrategy() const;

    void SetNextSpace(std::size_t space);

    void SetNextLineBreak(std::size_t line);

    void SetIndent(IndentStrategy strategy);

    std::size_t GetNextSpace();

    std::size_t GetNextLine();

    TokenStrategy GetTokenStrategy() const;

    IndentStrategy GetIndentStrategy() const;

    void Reset();
private:
    SpaceStrategy _spaceStrategy;
    TokenStrategy _tokenStrategy;
    IndentStrategy _indentStrategy;
    union {
        std::size_t Space;
        std::size_t Line;
    } _spaceData;
};
