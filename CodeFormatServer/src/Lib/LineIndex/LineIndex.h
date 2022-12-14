#pragma once

#include <vector>

#include "LineTypes.h"

class LineIndex {
public:
    LineIndex();

    void Parse(std::string &text);

    LineCol GetLineCol(std::size_t offset);

    std::size_t GetOffset(const LineCol& lineCol);

    std::size_t GetTotalLine();
private:
    std::vector<LineOffset> _newLines;
};
