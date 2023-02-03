#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "BasicSpellParserType.h"

namespace spell {

class IdentifyParser {
public:
    enum class IdentifyType {
        Unknown,
        Ascii,
        Unicode,
        Ignore,
        AsciiEnd,
        End,
    };

    IdentifyParser(std::string_view source);

    void Parse();

    std::vector<Word> &GetWords();

private:
    IdentifyType Lex();

    int GetCurrentChar();

    void PushWords(spell::WordRange range);

    std::string_view _source;
    std::size_t _currentIndex;
    std::vector<Word> _words;
};

}
