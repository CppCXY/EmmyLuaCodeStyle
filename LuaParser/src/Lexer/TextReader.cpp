#include "LuaParser/Lexer/TextReader.h"
#include "LuaParser/Lexer/LuaDefine.h"


TextReader::TextReader(std::string_view text)
        : _text(text),
          _hasSaveText(false),
          _buffStart(0), _buffIndex(0),
          _isEof(false),
          _currentIndex(0) {
}

int TextReader::NextChar() {
    if (_currentIndex < (_text.size() - 1)) {
        return _text[++_currentIndex];
    } else {
        ++_currentIndex;
        return EOZ;
    }
}

void TextReader::SaveAndNext() {
    Save();
    int ch = NextChar();
    if (ch == EOZ) {
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
    return EOZ;
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
    return _currentIndex;
}

TextRange TextReader::GetTokenRange() {
    return TextRange(_buffStart, _buffIndex);
}

void TextReader::ResetBuffer() {
    _hasSaveText = false;
    _buffStart = 0;
    _buffIndex = 0;
}

std::string_view TextReader::GetSaveText() const {
    if(_hasSaveText) {
        return _text.substr(_buffStart, _buffIndex - _buffStart + 1);
    }
    return _text.substr(_buffStart, 0);
}

bool TextReader::IsEof() const {
    return _isEof;
}
