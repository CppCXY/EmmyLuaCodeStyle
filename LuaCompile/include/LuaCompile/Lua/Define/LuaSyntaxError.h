#pragma once

#include "LuaCompile/Lib/TextRange/TextRange.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"
#include <string>

class LuaSyntaxError {
public:
    LuaSyntaxError(std::string_view errorMessage, TextRange range)
		: Message(errorMessage),
          Range(range)
	{
	}

	std::string Message;
	TextRange Range;
};
