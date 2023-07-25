#include "CodeFormatCore/Diagnostic/Spell/Util.h"
#include "Util/StringUtil.h"

using namespace spell;
const int EOZ = -1;

std::vector<Word> identify::ParseToWords(std::string_view identify) {
    TextReader reader(identify);
    std::vector<Word> words;
    while (true) {
        switch (Lex(reader)) {
            case IdentifyType::Unknown: {
                words.clear();
                goto endLoop;
            }
            case IdentifyType::Ascii: {
                auto range = reader.GetTokenRange();
                if (range.Length > 3) {
                    auto text = reader.GetSaveText();
                    words.emplace_back(range, text);
                }
                break;
            }
            case IdentifyType::Ignore: {
                break;
            }
            default:// end
            {
                goto endLoop;
            }
        }
    }
endLoop:
    return words;
}

identify::IdentifyType identify::Lex(TextReader &reader) {
    reader.ResetBuffer();
    do {
        auto ch = reader.GetCurrentChar();

        if (ch == EOZ) {
            return IdentifyType::End;
        } else if (ch == '_') {
            reader.NextChar();
            continue;
        } else if (ch > CHAR_MAX || !std::isalnum(ch)) {
            return IdentifyType::Unknown;
        }
        reader.SaveAndNext();

        if (islower(ch)) {
            reader.EatWhile(islower);
            return IdentifyType::Ascii;
        } else if (isdigit(ch)) {
            reader.EatWhile(isdigit);
            return IdentifyType::Ignore;
        } else if (isupper(ch)) {
            auto upperCount = reader.EatWhile(isupper);
            reader.EatWhile(islower);
            return upperCount > 0 ? IdentifyType::Ignore : IdentifyType::Ascii;
        }

        return IdentifyType::Unknown;
    } while (true);
}

std::vector<Word> text::ParseToIdentifies(std::string_view text) {
    std::vector<Word> words;
    if (text.length() < 3) {
        return words;
    }

    // give up check long string
    if (text.front() == '\'' || text.front() == '\"') {
        text = text.substr(1, text.size() - 2);
    } else {
        return words;
    }

    TextReader reader(text);

    while (true) {
        switch (Lex(reader)) {
            case TextType::Unknown: {
                words.clear();
                goto endLoop;
            }
            case TextType::Identify: {
                auto range = reader.GetTokenRange();
                if (range.Length > 3) {
                    range.StartOffset++;
                    words.emplace_back(range, reader.GetSaveText());
                }
                break;
            }
            case TextType::Ignore: {
                break;
            }
            default:// end
            {
                goto endLoop;
            }
        }
    }
endLoop:
    return words;
}

text::TextType text::Lex(TextReader &reader) {
    reader.ResetBuffer();
    do {
        auto ch = reader.GetCurrentChar();

        if (ch == EOZ) {
            return TextType::End;
        } else if (ch == '\\') {
            reader.NextChar();
            reader.NextChar();
            continue;
        } else if (::isspace(ch)) {
            reader.NextChar();
            continue;
        }
        reader.SaveAndNext();
        reader.EatWhile([](int ch) {
            return !isspace(ch);
        });
        return TextType::Identify;
    } while (true);
}