#pragma once

#include <cinttypes>
#include <vector>
#include "Util/Utf8.h"

struct LineCol {
    LineCol(std::size_t line, std::size_t col);

    std::size_t Line;
    std::size_t Col;
};

struct UnitChars {
    UnitChars(std::size_t totalLen, std::size_t charsNum, std::size_t unit);

    void Increment();

    std::size_t TotalLen;
    std::size_t CharsNum;
    std::size_t Unit;
};

class LineOffset {
public:
    explicit LineOffset(std::size_t start);

    void Push(std::size_t cLen);

    std::size_t GetCol(std::size_t colOffset);

    std::size_t GetOffset(std::size_t colNum);

    std::size_t Start;
    std::vector<UnitChars> CharsOffsets;

};
