#pragma once

#include <string_view>
#include "LuaTokenType.h"
#include "TextRange.h"
#include "LuaError.h"

class LuaToken
{
public:
	LuaToken(LuaTokenType type, std::string_view text, TextRange range)
		: TokenType(type),
		  Text(text),
		  Range(range)
	{
	}

	LuaTokenType TokenType;
	std::string_view Text;
	TextRange Range;
};
