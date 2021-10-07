#pragma once

#include <string_view>
#include "LuaTokenType.h"
#include "TextRange.h"

class LuaToken
{
public:
	LuaToken(LuaTokenType type, std::string_view text, TextRange range)
		: TokenType(type),
		  Text(text),
		  TextRange(range)
	{
	}

	LuaTokenType TokenType;
	std::string_view Text;
	TextRange TextRange;
};
