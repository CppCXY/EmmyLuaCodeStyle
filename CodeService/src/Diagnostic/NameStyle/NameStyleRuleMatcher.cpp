#include "CodeService/Diagnostic/NameStyle/NameStyleRuleMatcher.h"
#include "Util/StringUtil.h"


bool NameStyleRuleMatcher::Match(LuaSyntaxNode &n, const LuaSyntaxTree &t, const std::vector<NameStyleRule> &rules) {
    if (rules.empty()) {
        return false;
    }

    for (auto &rule: rules) {
        switch (rule.Type) {
            case NameStyleType::SnakeCase: {
                if (SnakeCase(n, t)) {
                    return true;
                }
                break;
            }
            case NameStyleType::UpperSnakeCase: {
                if (UpperSnakeCase(n, t)) {
                    return true;
                }
                break;
            }
            case NameStyleType::CamelCase: {
                if (CamelCase(n, t)) {
                    return true;
                }
                break;
            }
            case NameStyleType::PascalCase: {
                if (PascalCase(n, t)) {
                    return true;
                }
                break;
            }
            case NameStyleType::Same: {
                if (Same(n, t, rule.Param)) {
                    return true;
                }
                break;
            }
            case NameStyleType::Pattern: {
                if (PatternMatch(n, t, rule.Param)) {
                    return true;
                }
                break;
            }
            default: {
                break;
            }
        }
    }
    return false;
}

//void NameStyleRuleMatcher::ParseRule(std::string_view rule) {
//    auto file = std::make_shared<LuaFile>("", std::string(rule));
//    auto tokenParser = std::make_shared<LuaTokenParser>(file);
//    tokenParser->Parse();
//
//    while (tokenParser->Current().TokenType != TK_EOS) {
//        if (tokenParser->Current().TokenType == TK_NAME) {
//            auto &lookAhead = tokenParser->LookAhead();
//            if (lookAhead.TokenType == '(') {
//                NameStyleType type = NameStyleType::Same;
//                auto functionName = tokenParser->Current().Text;
//                if (functionName != "same") {
//                    type = NameStyleType::Custom;
//                }
//                std::vector<std::string> param;
//                tokenParser->Next();
//                while (tokenParser->Current().TokenType != ')' && tokenParser->Current().TokenType != TK_EOS) {
//                    if (tokenParser->Current().TokenType == TK_NAME || tokenParser->Current().TokenType == TK_STRING) {
//                        param.push_back(std::string(tokenParser->Current().Text));
//                    }
//                    tokenParser->Next();
//                }
//
//                _rulers.emplace_back(type, param);
//                tokenParser->Next();
//            } else if (lookAhead.TokenType == TK_EOS || lookAhead.TokenType == '|') {
//                auto currentText = tokenParser->Current().Text;
//                if (currentText == "snake_case") {
//                    _rulers.emplace_back(NameStyleType::SnakeCase);
//                } else if (currentText == "camel_case") {
//                    _rulers.emplace_back(NameStyleType::CamelCase);
//                } else if (currentText == "pascal_case") {
//                    _rulers.emplace_back(NameStyleType::PascalCase);
//                } else if (currentText == "upper_snake_case") {
//                    _rulers.emplace_back(NameStyleType::UpperSnakeCase);
//                }
//            }
//
//            if (lookAhead.TokenType == '|') {
//                tokenParser->Next();
//            }
//        } else // 语法错误
//        {
//            break;
//        }
//
//        tokenParser->Next();
//    }
//}

bool NameStyleRuleMatcher::SnakeCase(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    enum class ParseState {
        None,
        PrefixUnderscore,
        PrefixUnderscore2,
        Underscore,
        Letter,
        EndDigit
    } state = ParseState::None;

    auto source = n.GetText(t);
    for (std::size_t index = 0; index != source.size(); index++) {
        char ch = source[index];
        if (ch < 0) {
            return false;
        }

        switch (state) {
            case ParseState::None: {
                if (ch == '_') {
                    state = ParseState::PrefixUnderscore;
                } else if (::islower(ch)) {
                    state = ParseState::Letter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::PrefixUnderscore: {
                if (ch == '_') {
                    state = ParseState::PrefixUnderscore2;
                } else if (::islower(ch)) {
                    state = ParseState::Letter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::PrefixUnderscore2: {
                if (::islower(ch)) {
                    state = ParseState::Letter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::Underscore: {
                if (::islower(ch)) {
                    state = ParseState::Letter;
                } else {
                    return false;
                }

                break;
            }
            case ParseState::Letter: {
                if (::islower(ch)) {
                    // ignore
                } else if (ch == '_') {
                    state = ParseState::Underscore;
                } else if (::isdigit(ch)) {
                    state = ParseState::EndDigit;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::EndDigit: {
                if (!::isdigit(ch)) {
                    return false;
                }
                break;
            }
        }
    }
    return true;
}

bool NameStyleRuleMatcher::UpperSnakeCase(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    enum class ParseState {
        None,
        Underscore,
        Letter,
        EndDigit
    } state = ParseState::None;

    auto source = n.GetText(t);
    for (std::size_t index = 0; index != source.size(); index++) {
        char ch = source[index];
        if (ch < 0) {
            return false;
        }

        switch (state) {
            case ParseState::None: {
                if (::isupper(ch)) {
                    state = ParseState::Letter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::Underscore: {
                if (::isupper(ch)) {
                    state = ParseState::Letter;
                } else {
                    return false;
                }

                break;
            }
            case ParseState::Letter: {
                if (::isupper(ch)) {
                    // ignore
                } else if (ch == '_') {
                    state = ParseState::Underscore;
                } else if (::isdigit(ch)) {
                    state = ParseState::EndDigit;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::EndDigit: {
                if (!::isdigit(ch)) {
                    return false;
                }
                break;
            }
        }
    }
    return true;
}

bool NameStyleRuleMatcher::CamelCase(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    enum class ParseState {
        None,
        PrefixUnderscore,
        PrefixUnderscore2,
        LowerLetter,
        UpperLetter
    } state = ParseState::None;

    auto source = n.GetText(t);
    for (std::size_t index = 0; index != source.size(); index++) {
        char ch = source[index];
        if (ch < 0) {
            return false;
        }

        switch (state) {
            case ParseState::None: {
                if (ch == '_') {
                    state = ParseState::PrefixUnderscore;
                } else if (::islower(ch)) {
                    state = ParseState::LowerLetter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::PrefixUnderscore: {
                if (ch == '_') {
                    state = ParseState::PrefixUnderscore2;
                } else if (::islower(ch)) {
                    state = ParseState::LowerLetter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::PrefixUnderscore2: {
                if (::islower(ch)) {
                    state = ParseState::LowerLetter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::LowerLetter: {
                if (::islower(ch) || ::isdigit(ch)) {
                    // ignore
                } else if (::isupper(ch)) {
                    state = ParseState::UpperLetter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::UpperLetter: {
                if (::isupper(ch) || ::isdigit(ch)) {
                    // ignore
                } else if (::islower(ch)) {
                    state = ParseState::LowerLetter;
                } else {
                    return false;
                }
                break;
            }
        }
    }
    return true;
}

bool NameStyleRuleMatcher::PascalCase(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    enum class ParseState {
        None,
        LowerLetter,
        UpperLetter
    } state = ParseState::None;

    auto source = n.GetText(t);
    for (std::size_t index = 0; index != source.size(); index++) {
        char ch = source[index];
        if (ch < 0) {
            return false;
        }

        switch (state) {
            case ParseState::None: {
                if (::isupper(ch)) {
                    state = ParseState::UpperLetter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::LowerLetter: {
                if (::islower(ch) || ::isdigit(ch)) {
                    // ignore
                } else if (::isupper(ch)) {
                    state = ParseState::UpperLetter;
                } else {
                    return false;
                }
                break;
            }
            case ParseState::UpperLetter: {
                if (::isupper(ch) || ::isdigit(ch)) {
                    // ignore
                } else if (::islower(ch)) {
                    state = ParseState::LowerLetter;
                } else {
                    return false;
                }
                break;
            }
        }
    }
    return true;
}

bool NameStyleRuleMatcher::Same(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::string_view param) {
    return n.GetText(t) == param;
}

bool NameStyleRuleMatcher::PatternMatch(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::string_view pattern) {
    return string_util::FileWildcardMatch(n.GetText(t), pattern);
}


