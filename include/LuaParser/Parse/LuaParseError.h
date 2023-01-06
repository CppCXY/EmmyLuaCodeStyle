#pragma once

#include <string_view>
#include "LuaParser/Types/TextRange.h"

enum class LuaParserErrorKind {
    None,
    Expect,
    Match
};

class LuaParseError
{
public:
    LuaParseError(std::string_view errorMessage, TextRange range)
            : ErrorMessage(errorMessage),
              ErrorRange(range)
    {
    }

    std::string ErrorMessage;
    TextRange ErrorRange;
};
