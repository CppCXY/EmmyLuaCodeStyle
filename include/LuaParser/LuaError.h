#pragma once

#include <string>
#include "TextRange.h"


class LuaError
{
public:
	LuaError(std::string_view errorMessage, TextRange range, LuaTokenType token)
		: ErrorMessage(errorMessage),
		  ErrorRange(range),
		  MissToken(token)
	{
	}


	std::string ErrorMessage;
	TextRange ErrorRange;
	LuaTokenType MissToken;
};
