#include "CodeService/Diagnostic/Spell/TextParser.h"
#include "CodeService/Diagnostic/Spell/IdentifyParser.h"

using namespace spell;

bool IsIdentifier(char ch) {
    return ch > 0 && (std::isalnum(ch) || ch == '_');
}

TextParser::TextParser(std::string_view source)
        : _source(source) {
}

void TextParser::Parse() {
    enum class ParseState {
        Unknown,
        Identify
    } state = ParseState::Unknown;

    std::size_t start = 0;
    for (std::size_t i = 0; i != _source.size(); i++) {
        char ch = _source[i];
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
                    PushIdentifier(WordRange(start, i - start));
                }
                break;
            }
        }
    }
    if (state == ParseState::Identify) {
        PushIdentifier(WordRange(start, _source.size() - start));
    }
}

std::vector<Word> &TextParser::GetIdentifiers() {
    return _identifiers;
}

void TextParser::PushIdentifier(spell::WordRange range) {
    if (range.Count <= 3) {
        return;
    }

    std::string_view identifyView = _source.substr(range.Start, range.Count);
    _identifiers.emplace_back(range, std::string(identifyView));
}
