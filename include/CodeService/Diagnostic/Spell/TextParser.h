#pragma once

#include "BasicSpellParserType.h"
#include <string_view>
#include <vector>

namespace spell {
// TODO Write later
class TextParser {
public:
    TextParser(std::string_view source);

    void Parse();

    std::vector<Word> &GetIdentifiers();

private:
    void PushIdentifier(spell::WordRange range);

    std::string_view _source;
    std::vector<Word> _identifiers;
};
}
