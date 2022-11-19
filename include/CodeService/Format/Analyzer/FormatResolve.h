#pragma once

#include "FormatStrategy.h"
#include <cinttypes>

class FormatResolve {
public:
    FormatResolve();

    NextSpaceStrategy GetNextSpaceStrategy() const;

    void SetNextSpace(std::size_t space);

    void SetNextLineBreak(std::size_t line);

    void SetIndent(std::size_t indent = 0);

    std::size_t GetNextSpace();

    std::size_t GetNextLine();

    TokenStrategy GetTokenStrategy() const;

    std::size_t GetIndent() const;

    void Reset();
private:
    NextSpaceStrategy _spaceStrategy;
    TokenStrategy _tokenStrategy;
    union Data {
        std::size_t Space;
        std::size_t Line;
        std::size_t Indent;
    } _data;
};
