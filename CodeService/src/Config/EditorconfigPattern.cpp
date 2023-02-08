#include "CodeService/Config/EditorconfigPattern.h"
#include <algorithm>
#include "LuaParser/Lexer/LuaDefine.h"
#include "Util/StringUtil.h"


EditorconfigPattern::EditorconfigPattern() {

}

void EditorconfigPattern::Compile(std::string_view pattern) {
    _patternSource = pattern;
    TextReader reader(_patternSource);
    while (true) {
        auto type = Lex(reader);
        if (type == MatchType::Eof) {
            break;
        }

        auto &matchData = _matches.emplace_back(type);
        auto text = reader.GetSaveText();
        switch (type) {
            case MatchType::Path: {
                matchData.String = text;
                break;
            }
            case MatchType::AnyCharOf: {
                if (text.size() > 2) {
                    text = text.substr(1, text.size() - 2);
                    matchData.CharSet = std::set<char>(text.begin(), text.end());
                }
                break;
            }
            case MatchType::NotCharOf: {
                if (text.size() > 3) {
                    text = text.substr(2, text.size() - 3);
                    matchData.CharSet = std::set<char>(text.begin(), text.end());
                }
                break;
            }
            case MatchType::StringOf: {
                if (text.size() > 2) {
                    text = text.substr(1, text.size() - 2);
                    matchData.Strings = string_util::Split(text, ",");
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

EditorconfigPattern::MatchType EditorconfigPattern::Lex(TextReader &reader) {
    reader.ResetBuffer();
    int ch = reader.GetCurrentChar();
    switch (ch) {
        case '*': {
            reader.SaveAndNext();
            if (reader.GetCurrentChar() == '*') {
                reader.SaveAndNext();
                return MatchType::AnyChars;
            }

            return MatchType::AnyCharsExceptSep;
        }
        case '{': {
            reader.SaveAndNext();
            while (!reader.IsEof()) {
                ch = reader.GetCurrentChar();
                reader.SaveAndNext();
                if (ch == '}') {
                    break;
                }
            }
            return MatchType::StringOf;
        }
        case '[': {
            reader.SaveAndNext();
            MatchType type = MatchType::AnyCharOf;
            if (reader.GetCurrentChar() == '!') {
                type = MatchType::NotCharOf;
                reader.SaveAndNext();
            }
            while (!reader.IsEof()) {
                ch = reader.GetCurrentChar();
                reader.SaveAndNext();
                if (ch == ']') {
                    break;
                }
            }

            return type;
        }
        case '?': {
            reader.SaveAndNext();
            return MatchType::AnyChar;
        }
        case EOZ: {
            return MatchType::Eof;
        }
        default: {
            reader.SaveAndNext();
            while (!reader.IsEof()) {
                ch = reader.GetCurrentChar();
                if (ch == '[' || ch == '{' || ch == '*') {
                    break;
                }
                reader.SaveAndNext();
            }

            return MatchType::Path;
        }
    }
}

bool EditorconfigPattern::Match(std::string_view filePath) {
    if (filePath.empty()) {
        return false;
    }
    std::string newPath(filePath);
    for (auto &c: newPath) {
        if (c == '\\') {
            c = '/';
        }
    }
    std::string pathView = newPath;

    std::size_t matchProcess = _matches.size();
    for (; matchProcess > 0; matchProcess--) {
        auto &matchData = _matches[matchProcess - 1];
        switch (matchData.Type) {
            case MatchType::Path: {
                if (!string_util::EndWith(pathView, matchData.String)) {
                    return false;
                }
                pathView = pathView.substr(0, pathView.size() - matchData.String.size());
                break;
            }
            case MatchType::AnyCharsExceptSep: {
                std::size_t matchIndex = pathView.size();
                for (; matchIndex > 0; matchIndex--) {
                    char ch = pathView[matchIndex - 1];
                    if (ch == '/' || ch == '\\') {
                        break;
                    }
                }

                if (matchProcess > 1) {
                    auto &nextMatchData = _matches[matchProcess - 2];
                    if (nextMatchData.Type == MatchType::Path) {
                        auto pos = pathView.rfind(nextMatchData.String);
                        if (pos == std::string_view::npos) {
                            return false;
                        }
                        if (pos + nextMatchData.String.size() < matchIndex) {
                            return false;
                        }
                        matchIndex = pos + nextMatchData.String.size();
                    }
                }

                pathView = pathView.substr(0, matchIndex);
                break;
            }
            case MatchType::AnyChars: {
                std::size_t matchIndex = 0;
                if (matchProcess > 1) {
                    auto &nextMatchData = _matches[matchProcess - 2];
                    if (nextMatchData.Type == MatchType::Path) {
                        auto pos = pathView.rfind(nextMatchData.String);
                        if (pos == std::string_view::npos) {
                            return false;
                        }
                        matchIndex = pos + nextMatchData.String.size();
                    }
                }

                pathView = pathView.substr(0, matchIndex);
                break;
            }
            case MatchType::AnyChar: {
                if (pathView.empty()) {
                    return false;
                }
                pathView = pathView.substr(0, pathView.size() - 1);
                break;
            }
            case MatchType::AnyCharOf: {
                if (pathView.empty()) {
                    return false;
                }

                if (matchData.CharSet.count(pathView.back()) == 0) {
                    return false;
                }
                pathView = pathView.substr(0, pathView.size() - 1);
                break;
            }
            case MatchType::NotCharOf: {
                if (pathView.empty()) {
                    return false;
                }

                if (matchData.CharSet.count(pathView.back()) > 0) {
                    return false;
                }
                pathView = pathView.substr(0, pathView.size() - 1);
                break;
            }
            case MatchType::StringOf: {
                bool match = false;
                for (auto &s: matchData.Strings) {
                    if (string_util::EndWith(pathView, s)) {
                        match = true;
                        pathView = pathView.substr(0, pathView.size() - s.size());
                        break;
                    }
                }

                if (!match) {
                    return false;
                }
                break;
            }
            default: {
                return false;
            }
        }
    }

    return matchProcess == 0;
}

std::string_view EditorconfigPattern::GetPattern() {
    return _patternSource;
}


