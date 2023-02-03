#pragma once

#include <string_view>
#include "LuaTokenKind.h"
#include "LuaParser/Types/TextRange.h"

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
