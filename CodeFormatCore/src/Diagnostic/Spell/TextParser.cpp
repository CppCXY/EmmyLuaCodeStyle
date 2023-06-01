#include "CodeFormatCore/Diagnostic/Spell/TextParser.h"
using namespace spell;

bool IsIdentifier(char ch) {
    return ch > 0 && (std::isalnum(ch) || ch == '_');
}

std::vector<Word> TextParser::ParseToWords(std::string_view source) {
    enum class ParseState {
        Unknown,
        Identify
    } state = ParseState::Unknown;
    std::vector<Word> words;
    std::size_t start = 0;
    for (std::size_t i = 0; i != source.size(); i++) {
        char ch = source[i];
        switch (state) {
            case ParseState::Unknown: {
                if (IsIdentifier(ch)) {
                    state = ParseState::Identify;
                    start = i;
                }
                break;
            }
            case ParseState::Identify: {
                if (!IsIdentifier(ch)) {
                    state = ParseState::Unknown;
                    auto len = i - start;
                    if (len <= 3) {
                        break;
                    }
                    auto range = WordRange(start, len);
                    words.emplace_back(range, source.substr(start, len));
                }
                break;
            }
        }
    }
    if (state == ParseState::Identify) {
        auto len = source.size() - start;
        if (len > 3) {
            auto range = WordRange(start, len);
            words.emplace_back(range, source.substr(start, len));
        }
    }
    return words;
}
