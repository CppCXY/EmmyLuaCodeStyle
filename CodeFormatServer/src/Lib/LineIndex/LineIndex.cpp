#include "LineIndex.h"
#include <algorithm>

LineIndex::LineIndex() {

}

void LineIndex::Parse(std::string &text) {
    _newLines.clear();
    _newLines.resize(1, LineOffset(0));
    for (std::size_t i = 0; i < text.size();) {
        char c = text[i];
        if (c > 0) {
            std::size_t cLen = sizeof(char);
            _newLines.back().Push(cLen);
            if (c == '\n') {
                _newLines.emplace_back(i + 1);
            }
            i++;
        } else {
            std::size_t cLen = utf8::Utf8OneCharLen(text.data() + i);
            i += cLen;
            _newLines.back().Push(cLen);
        }
    }
}

LineCol LineIndex::GetLineCol(std::size_t offset) {
    auto lineIt = std::partition_point(
            _newLines.begin(), _newLines.end(),
            [offset](LineOffset &lo) {
                return lo.Start <= offset;
            });
    std::size_t line = 0;
    if (lineIt > _newLines.begin()) {
        line = static_cast<std::size_t>(lineIt - _newLines.begin() - 1);
    }
    auto lineStartOffset = _newLines.at(line).Start;
    auto colOffset = offset - lineStartOffset;
    auto col = _newLines.at(line).GetCol(colOffset);
    return {line, col};
}

std::size_t LineIndex::GetOffset(const LineCol &lineCol) {
    if (lineCol.Line < _newLines.size()) {
        return _newLines.at(lineCol.Line).GetOffset(lineCol.Col);
    }
    return 0;
}

std::size_t LineIndex::GetTotalLine() {
    return _newLines.size();
}
