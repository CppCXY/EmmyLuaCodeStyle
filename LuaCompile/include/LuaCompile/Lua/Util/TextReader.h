#pragma once
#include "LuaCompile/Lib/TextRange/TextRange.h"
#include <string_view>
#include <functional>

class TextReader {
public:
    explicit TextReader(std::string_view text, std::size_t offset = 0);

    int NextChar();

    void SaveAndNext();

    void Save();

    int GetCurrentChar();

    bool CheckNext1(int ch);

    bool CheckNext2(std::string_view set);

    std::size_t GetPos();

    TextRange GetTokenRange();

    std::string_view GetSaveText() const;

    std::size_t EatWhen(int ch);

    std::size_t EatWhile(const std::function<bool(int ch)>& fn);

    bool IsEof() const;

    void ResetBuffer();
private:
    std::string_view _text;

    bool _hasSaveText;
    std::size_t _buffStart;
    std::size_t _buffIndex;

    bool _isEof;
    std::size_t _currentIndex;
    std::size_t _offset;
};