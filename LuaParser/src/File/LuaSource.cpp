#include "LuaParser/File/LuaSource.h"

#include "Util/Utf8.h"

std::shared_ptr<LuaSource> LuaSource::From(std::string &&source) {
    return std::make_shared<LuaSource>(std::move(source));
}

LuaSource::LuaSource(std::string &&fileText)
    : _source(fileText),
      _linenumber(0),
      _lineState(EndOfLine::UNKNOWN) {
    _lineOffsetVec.push_back(0);
}

std::size_t LuaSource::GetLine(std::size_t offset) const {
    if (_lineOffsetVec.empty()) {
        return 0;
    }

    int maxLine = static_cast<int>(_lineOffsetVec.size()) - 1;
    int targetLine = maxLine;
    int upperLine = maxLine;
    int lowestLine = 0;

    while (true) {
        if (_lineOffsetVec[targetLine] > offset) {
            upperLine = targetLine;

            targetLine = (upperLine + lowestLine) / 2;

            if (targetLine == 0) {
                return targetLine;
            }
        } else {
            if (upperLine - targetLine <= 1) {
                return targetLine;
            }

            lowestLine = targetLine;

            targetLine = (upperLine + lowestLine) / 2;
        }
    }

    return 0;
}

std::size_t LuaSource::GetColumn(std::size_t offset) const {
    auto line = GetLine(offset);

    auto lineStartOffset = _lineOffsetVec[line];

    if (offset > lineStartOffset) {
        auto bytesLength = offset - lineStartOffset;
        return utf8::Utf8nLen(_source.data() + lineStartOffset,
                              bytesLength);
    }
    return 0;
}

bool LuaSource::CheckCurrentLineUnicodeBefore(std::size_t offset) const {
    auto line = GetLine(offset);

    auto lineStartOffset = _lineOffsetVec[line];

    if (offset > lineStartOffset) {
        for (std::size_t i = lineStartOffset; i < offset; i++) {
            if (_source[i] & 0x80) {
                return true;
            }
        }
    }
    return false;
}

std::size_t LuaSource::GetLineOffset(std::size_t offset) const {
    auto line = GetLine(offset);

    auto lineStartOffset = _lineOffsetVec[line];

    if (offset > lineStartOffset) {
        return offset - lineStartOffset;
    }
    return 0;
}

std::size_t LuaSource::GetOffset(std::size_t line, std::size_t character) const {
    std::size_t sizeLine = line;

    if (sizeLine >= _lineOffsetVec.size()) {
        return _source.size() + 1;
    }

    auto lineStartOffset = _lineOffsetVec[line];
    std::size_t nextOffset = 0;
    if (sizeLine + 1 >= _lineOffsetVec.size()) {
        nextOffset = _source.size();
    } else {
        nextOffset = _lineOffsetVec[line + 1];
    }

    std::size_t offset = utf8::Utf8nByteNum(_source.data() + lineStartOffset, nextOffset - lineStartOffset, character);
    return lineStartOffset + offset;
}

std::size_t LuaSource::GetTotalLine() const {
    return _linenumber;
}

void LuaSource::PushLine(std::size_t offset) {
    _lineOffsetVec.push_back(offset);
    _linenumber++;
}

std::string_view LuaSource::GetSource() const {
    return _source;
}

std::string_view LuaSource::Slice(std::size_t startOffset, std::size_t endOffset) const {
    std::string_view source = _source;
    return source.substr(startOffset, endOffset - startOffset + 1);
}

std::string_view LuaSource::Slice(TextRange range) const {
    if (range.Length == 0) {
        return "";
    }
    return Slice(range.StartOffset, range.GetEndOffset());
}

void LuaSource::SetTotalLine(std::size_t line) {
    _linenumber = line;
}

void LuaSource::UpdateLineInfo(std::size_t startLine) {
    auto totalLine = GetTotalLine();
    std::size_t startOffset = 0;
    if (totalLine < startLine) {
        startLine = 0;
    } else {
        startOffset = _lineOffsetVec[startLine];
    }

    _lineOffsetVec.resize(startLine + 1);

    for (; startOffset < _source.size(); startOffset++) {
        if (_source[startOffset] == '\n') {
            _lineOffsetVec.push_back(startOffset + 1);
        }
    }
}

void LuaSource::Reset() {
    _lineOffsetVec.resize(0);
    _lineOffsetVec.push_back(0);
    _linenumber = 0;
    _lineState = EndOfLine::UNKNOWN;
}

void LuaSource::SetEndOfLineState(EndOfLine endOfLine) {
    switch (_lineState) {
        case EndOfLine::UNKNOWN: {
            _lineState = endOfLine;
            break;
        }
        case EndOfLine::CR:
        case EndOfLine::LF:
        case EndOfLine::CRLF: {
            if (_lineState != endOfLine) {
                _lineState = EndOfLine::MIX;
            }
            break;
        }
        default: {
            break;
        }
    }
}

EndOfLine LuaSource::GetEndOfLine() const {
    return _lineState;
}

std::size_t LuaSource::GetLineRestCharacter(std::size_t offset) {
    auto line = GetLine(offset);
    std::size_t bytesLength = 0;
    if (line + 1 < _lineOffsetVec.size()) {
        auto nextOffset = _lineOffsetVec[line + 1];
        bytesLength = nextOffset - offset;
    } else {
        bytesLength = _source.size() - 1 - offset;
    }
    if (bytesLength > 0) {
        return utf8::Utf8nLen(_source.data() + offset, bytesLength);
    } else {
        return 0;
    }
}

TextRange LuaSource::GetIndentRange(std::size_t offset) const {
    std::string_view source = GetSource();
    auto line = GetLine(offset);
    const auto start = GetOffset(line, 0);

    auto index = start;
    for (; index < offset; index++) {
        auto ch = source[index];
        if (ch != '\t' && ch != ' ') {
            return TextRange(start, index - start);
        }
    }

    return TextRange(start, index - start);
}

bool LuaSource::IsEmptyLine(std::size_t line) const {
    if (line >= _lineOffsetVec.size()) {
        return true;
    }

    std::size_t lineStartOffset = _lineOffsetVec[line];
    std::size_t nextLineStartOffset = 0;

    if (line + 1 == _lineOffsetVec.size()) {
        nextLineStartOffset = _source.size();
    } else {
        nextLineStartOffset = _lineOffsetVec[line + 1];
    }

    for (auto offset = lineStartOffset; offset < nextLineStartOffset; offset++) {
        char ch = _source[offset];
        if (ch != '\n' && ch != '\r' && ch != '\t' && ch != ' ') {
            return false;
        }
    }

    return true;
}
