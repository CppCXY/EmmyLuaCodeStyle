#include "CodeFormatCore/Diagnostic/Spell/CodeSpellChecker.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"
#include "CodeFormatCore/Diagnostic/DiagnosticType.h"
#include "CodeFormatCore/Diagnostic/Spell/Util.h"
#include "LuaParser/Lexer/LuaToken.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "Util/format.h"

CodeSpellChecker::CodeSpellChecker()
    : _symSpell(std::make_shared<SymSpell>(SymSpell::Strategy::LazyLoaded)) {
}

void CodeSpellChecker::LoadDictionary(std::string_view path) {
    _symSpell->LoadWordDictionary(std::string(path));
}

void CodeSpellChecker::LoadDictionaryFromBuffer(std::string_view buffer) {
    _symSpell->LoadWordDictionaryFromBuffer(buffer);
}

void CodeSpellChecker::Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t) {
    for (auto &token: t.GetTokens()) {
        if (token.GetTokenKind(t) == TK_NAME) {
            IdentifyAnalyze(d, token, t);
        } else if (token.GetTokenKind(t) == TK_STRING) {
            TextAnalyze(d, token, t);
        }
    }
}

std::vector<SuggestItem> CodeSpellChecker::GetSuggests(std::string word) {
    enum class ParseState {
        Unknown,
        FirstUpper,
        AllUpper,
        Lower,
    } state = ParseState::Unknown;

    std::vector<SuggestItem> suggests;

    for (std::size_t i = 0; i != word.size(); i++) {
        char &c = word[i];
        if (c < 0 || !std::isalpha(c)) {
            return suggests;
        }

        switch (state) {
            case ParseState::Unknown: {
                if (std::isupper(c)) {
                    state = ParseState::AllUpper;
                    c = static_cast<char>(std::tolower(c));
                } else// lower
                {
                    state = ParseState::Lower;
                }

                break;
            }
            case ParseState::AllUpper: {
                if (std::islower(c)) {
                    if (i == 1) {
                        state = ParseState::FirstUpper;
                    } else {
                        return suggests;
                    }
                }
                c = static_cast<char>(std::tolower(c));
                break;
            }
            case ParseState::FirstUpper: {
                if (!std::islower(c)) {
                    return suggests;
                }
                break;
            }
            case ParseState::Lower: {
                if (std::isupper(c)) {
                    return suggests;
                }
                break;
            }
        }
    }

    if (state == ParseState::Unknown) {
        return suggests;
    }

    suggests = _symSpell->LookUp(word);

    switch (state) {
        case ParseState::FirstUpper: {
            for (auto &suggest: suggests) {
                if (!suggest.Term.empty()) {
                    suggest.Term[0] = std::toupper(suggest.Term[0]);
                }
            }
            break;
        }
        case ParseState::AllUpper: {
            for (auto &suggest: suggests) {
                if (!suggest.Term.empty()) {
                    for (auto &ch: suggest.Term) {
                        ch = std::toupper(ch);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }

    return suggests;
}

std::string lowerString(std::string_view source) {
    std::string lowerItem(source);
    std::transform(lowerItem.begin(), lowerItem.end(), lowerItem.begin(), ::tolower);
    return lowerItem;
}

void CodeSpellChecker::IdentifyAnalyze(DiagnosticBuilder &d, LuaSyntaxNode &token, const LuaSyntaxTree &t) {
    std::string text(token.GetText(t));

    auto &customDict = _dictionary;
    if (customDict.count(text) != 0) {
        return;
    }

    auto words = spell::identify::ParseToWords(text);
    if (words.empty()) {
        return;
    }
    for (auto &word: words) {
        auto lowerItem = lowerString(word.Item);
        if (!word.Item.empty() && customDict.count(lowerItem) == 0 && !_symSpell->IsCorrectWord(lowerItem)) {
            auto tokenRange = token.GetTextRange(t);
            auto range = TextRange(tokenRange.StartOffset + word.Range.StartOffset,
                                   word.Range.Length);
            std::string originText(text.substr(word.Range.StartOffset, word.Range.Length));
            d.PushDiagnostic(DiagnosticType::Spell, range,
                             util::format("Typo in identifier '{}'", originText), originText);
        }
    }
}

void CodeSpellChecker::TextAnalyze(DiagnosticBuilder &d, LuaSyntaxNode &token, const LuaSyntaxTree &t) {
    auto identifiers = spell::text::ParseToIdentifies(token.GetText(t));
    if (identifiers.empty()) {
        return;
    }
    auto &customDict = _dictionary;

    for (auto &identifier: identifiers) {
        auto identifyText = identifier.Item;
        auto words = spell::identify::ParseToWords(identifyText);
        if (words.empty()) {
            continue;
        }

        auto tokenRange = token.GetTextRange(t);
        for (auto &word: words) {
            auto lowerItem = lowerString(word.Item);
            if (!word.Item.empty() && !_symSpell->IsCorrectWord(lowerItem) && customDict.count(lowerItem) == 0) {
                auto range = TextRange(tokenRange.StartOffset + identifier.Range.StartOffset + word.Range.StartOffset,
                                       word.Range.Length);
                std::string originText(
                        token.GetText(t).substr(identifier.Range.StartOffset + word.Range.StartOffset, word.Range.Length));
                d.PushDiagnostic(DiagnosticType::Spell, range,
                                 util::format("Typo in string '{}'", originText), originText);
            }
        }
    }
}

void CodeSpellChecker::SetCustomDictionary(const CodeSpellChecker::CustomDictionary &dictionary) {
    _dictionary = dictionary;
}
