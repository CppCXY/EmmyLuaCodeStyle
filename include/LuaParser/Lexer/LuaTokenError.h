#pragma once

#include <string>
#include "LuaParser/Types/TextRange.h"
#include "LuaTokenType.h"

class LuaTokenError
{
public:
	LuaTokenError(std::string_view errorMessage, TextRange range, LuaTokenType token)
		: ErrorMessage(errorMessage),
		  ErrorRange(range),
		  MissToken(token)
	{
	}

	std::string ErrorMessage;
	TextRange ErrorRange;
	LuaTokenType MissToken;
};
