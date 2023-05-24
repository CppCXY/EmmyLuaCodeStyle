#pragma once

#include "BasicSpellParserType.h"
#include <string_view>
#include <vector>

namespace spell {
// TODO Write later
class TextParser {
public:
    static std::vector<Word> ParseToWords(std::string_view source);
};
}
