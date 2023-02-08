#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <set>
#include "LuaParser/Lexer/TextReader.h"

// editorconfig-core-c 的依赖太多了，我也不是不能自己写

class EditorconfigPattern {
public:
    enum class MatchType {
        Path, // originText
        AnyCharsExceptSep, // *
        AnyChars, // **
        AnyChar, // ?
        AnyCharOf, // [abc]
        NotCharOf, // [!abc]
        StringOf, // {js,lua,ts}
        NumRange, // {1..2}
        Eof
    };

    struct MatchData {
        MatchData(MatchType type)
                : Type(type) {}

        MatchType Type;
        // 我应该用 variant，但是我觉得一些电脑编译不过
        std::string_view String;
        std::vector<std::string_view> Strings;
        std::set<char> CharSet;
    };

    EditorconfigPattern();

    void Compile(std::string_view pattern);

    bool Match(std::string_view path);

    std::string_view GetPattern();
private:
    MatchType Lex(TextReader& reader);

    std::string _patternSource;
    std::vector<MatchData> _matches;
};