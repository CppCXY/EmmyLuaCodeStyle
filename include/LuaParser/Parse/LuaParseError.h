#pragma once

#include <string_view>
#include "LuaParser/Types/TextRange.h"

enum class LuaParserErrorKind {
    None,
    Expect
};

class LuaParseError
{
public:
    LuaParseError(std::string_view errorMessage, TextRange range, LuaTokenKind expectToken = 0)
            : ErrorMessage(errorMessage),
              ErrorRange(range),
              ExpectToken(expectToken)
    {
    }

    std::string ErrorMessage;
    TextRange ErrorRange;
    LuaTokenKind ExpectToken;
};
