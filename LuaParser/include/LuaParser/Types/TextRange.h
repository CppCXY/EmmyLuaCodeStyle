#pragma once

#include <cstdlib>

class TextRange {
public:
    TextRange();

    TextRange(std::size_t startOffset, std::size_t length);

    std::size_t GetEndOffset() const;

    std::size_t StartOffset;
    std::size_t Length;
};
