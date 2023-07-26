#pragma once

#include "LuaParser/Lexer/TextReader.h"
#include "LuaParser/Types/TextRange.h"
#include <cinttypes>
#include <string>
#include <string_view>
#include <vector>
#include <climits>

namespace spell {

struct Word {
    TextRange Range;
    std::string_view Item;

    Word(TextRange range, std::string_view item)
        : Range(range),
          Item(item) {
    }
};

namespace identify {
enum class IdentifyType {
    Unknown,
    Ascii,
    Ignore,
    End,
};

std::vector<Word> ParseToWords(std::string_view identify);

IdentifyType Lex(TextReader &reader);

};// namespace identify

namespace text {
enum class TextType {
    Unknown,
    Identify,
    Ignore,
    End,
};

std::vector<Word> ParseToIdentifies(std::string_view text);

TextType Lex(TextReader &reader);

}// namespace text

}// namespace spell
