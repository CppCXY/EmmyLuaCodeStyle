#pragma once

#include "LuaCompile/Lib/TextRange/TextRange.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"
#include <string_view>

class LuaToken
{
public:
	LuaToken(LuaTokenKind type, TextRange range)
		: TokenType(type),
		  Range(range)
	{
	}

	LuaTokenKind TokenType;
	TextRange Range;
};
