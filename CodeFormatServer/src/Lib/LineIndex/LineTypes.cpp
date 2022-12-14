#include "LineTypes.h"

LineCol::LineCol(std::size_t line, std::size_t col) : Line(line), Col(col) {}

void UnitChars::Increment() {
    TotalLen += Unit;
    CharsNum++;
}

UnitChars::UnitChars(std::size_t totalLen, std::size_t charsNum, std::size_t unit)
        : TotalLen(totalLen), CharsNum(charsNum), Unit(unit) {

}

LineOffset::LineOffset(std::size_t start)
        : Start(start) {

}

void LineOffset::Push(std::size_t cLen) {
    auto unit = cLen;
    if (CharsOffsets.empty() || CharsOffsets.back().Unit != unit) {
        CharsOffsets.emplace_back(
                unit, 1, unit
        );
    } else {
        CharsOffsets.back().Increment();
    }
}

std::size_t LineOffset::GetCol(std::size_t colOffset) {
    std::size_t col = 0;
    std::size_t offset = colOffset;
    for (auto &uc: CharsOffsets) {
        if (offset > uc.TotalLen) {
            offset -= uc.TotalLen;
            col += uc.CharsNum;
        } else {
            col += offset / uc.Unit;
            break;
        }
    }
    return col;
}

std::size_t LineOffset::GetOffset(std::size_t colNum) {
    std::size_t colOffset = 0;
    std::size_t num = colNum;
    for (auto &uc: CharsOffsets) {
        if (num > uc.CharsNum) {
            num -= uc.CharsNum;
            colOffset += uc.TotalLen;
        } else {
            colOffset += num * uc.Unit;
            break;
        }
    }
    return Start + colOffset;
}
