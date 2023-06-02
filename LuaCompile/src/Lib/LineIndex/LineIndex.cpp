#include "LuaCompile/Lib/LineIndex/LineIndex.h"
#include <algorithm>

LineIndex::LineIndex() {
}

void LineIndex::Parse(std::string &text, std::size_t startOffset) {
    std::size_t parseStart = 0;
    if (startOffset == 0) {
        _newLines.clear();
        _newLines.resize(1, LineOffset(0));
    } else {
        auto line = GetLine(startOffset);
        if (line == 0) {
            line = 1;
        }
        _newLines.resize(line);
        _newLines.back().Reset();
        parseStart = _newLines.back().Start;
    }

    for (std::size_t i = parseStart; i < text.size();) {
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

LineCol LineIndex::GetLineCol(std::size_t offset) const {
    auto line = GetLine(offset);
    auto lineStartOffset = _newLines.at(line).Start;
    auto colOffset = offset - lineStartOffset;
    auto col = _newLines.at(line).GetCol(colOffset);
    return {line, col};
}

std::size_t LineIndex::GetLine(std::size_t offset) const {
    auto lineIt = std::partition_point(
            _newLines.begin(), _newLines.end(),
            [offset](const LineOffset &lo) {
                return lo.Start <= offset;
            });
    std::size_t line = 0;
    if (lineIt > _newLines.begin()) {
        line = static_cast<std::size_t>(lineIt - _newLines.begin() - 1);
    }
    return line;
}

std::size_t LineIndex::GetCol(std::size_t offset) const {
    auto line = GetLine(offset);
    auto lineStartOffset = _newLines.at(line).Start;
    auto colOffset = offset - lineStartOffset;
    return _newLines.at(line).GetCol(colOffset);
}

std::size_t LineIndex::GetOffset(const LineCol &lineCol) const {
    if (lineCol.Line < _newLines.size()) {
        return _newLines.at(lineCol.Line).GetOffset(lineCol.Col);
    }
    return 0;
}

std::size_t LineIndex::GetTotalLine() const {
    return _newLines.size();
}

lsp::Range LineIndex::ToLspRange(TextRange range) const {
    auto p1 = ToLspPosition(range.StartOffset);
    auto p2 = ToLspPosition(range.GetEndOffset());
    p2.character++;
    return lsp::Range(p1, p2);
}

lsp::Position LineIndex::ToLspPosition(std::size_t offset) const {
    auto lc = GetLineCol(offset);
    return lsp::Position(lc.Line, lc.Col);
}
