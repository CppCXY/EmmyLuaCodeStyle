#pragma once

#include "LuaParser/Types/TextRange.h"
#include "LuaTokenKind.h"
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
