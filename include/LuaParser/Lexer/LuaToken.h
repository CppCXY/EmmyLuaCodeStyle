#pragma once

#include <string_view>
#include "LuaTokenType.h"
#include "LuaParser/Types/TextRange.h"

class LuaToken
{
public:
	LuaToken(LuaTokenType type, TextRange range)
		: TokenType(type),
		  Range(range)
	{
	}

	LuaTokenType TokenType;
	TextRange Range;
};
