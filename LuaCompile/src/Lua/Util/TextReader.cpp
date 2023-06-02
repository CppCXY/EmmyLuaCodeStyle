#include "LuaCompile/Lua/Util/TextReader.h"
#include "LuaCompile/Lua/Define/LuaDefine.h"


TextReader::TextReader(std::string_view text, std::size_t offset)
    : _text(text),
      _hasSaveText(false),
      _buffStart(0), _buffIndex(0),
      _isEof(false),
      _currentIndex(0),
      _offset(offset) {
}

int TextReader::NextChar() {
    if (_currentIndex < (_text.size() - 1)) {
        return _text[++_currentIndex];
    } else {
        ++_currentIndex;
        return EOF_CHAR;
    }
}

void TextReader::SaveAndNext() {
    Save();
    int ch = NextChar();
    if (ch == EOF_CHAR) {
        _isEof = true;
    }
}

void TextReader::Save() {
    if (!_hasSaveText) {
        _hasSaveText = true;
        _buffStart = _currentIndex;
    }
    _buffIndex = _currentIndex;
}

int TextReader::GetCurrentChar() {
    if (!_isEof && _currentIndex < _text.size()) {
        unsigned char ch = _text[_currentIndex];
        return ch;
    }
    return EOF_CHAR;
}

bool TextReader::CheckNext1(int ch) {
    if (_currentIndex < _text.size() && _text[_currentIndex] == ch) {
        SaveAndNext();
        return true;
    }
    return false;
}

bool TextReader::CheckNext2(std::string_view set) {
    if (set.size() != 2) {
        return false;
    }
    int ch = GetCurrentChar();
    if (ch == set[0] || ch == set[1]) {
        SaveAndNext();
        return true;
    }
    return false;
}

std::size_t TextReader::GetPos() {
    return _currentIndex + _offset;
}

TextRange TextReader::GetTokenRange() {
    return TextRange(_buffStart + _offset, _buffIndex - _buffStart + 1);
}

void TextReader::ResetBuffer() {
    _hasSaveText = false;
    _buffStart = 0;
    _buffIndex = 0;
}

std::string_view TextReader::GetSaveText() const {
    if (_hasSaveText) {
        return _text.substr(_buffStart, _buffIndex - _buffStart + 1);
    }
    return _text.substr(_buffStart, 0);
}

bool TextReader::IsEof() const {
    return _isEof;
}

std::size_t TextReader::EatWhen(int ch) {
    std::size_t count = 0;
    while (!IsEof() && GetCurrentChar() == ch) {
        SaveAndNext();
        count++;
    }
    return count;
}

std::size_t TextReader::EatWhile(const std::function<bool(int ch)>& fn) {
    std::size_t count = 0;
    while (!IsEof() && fn(GetCurrentChar())) {
        SaveAndNext();
        count++;
    }
    return count;
}
