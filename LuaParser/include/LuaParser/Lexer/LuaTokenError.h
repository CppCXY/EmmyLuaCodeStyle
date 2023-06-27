#pragma once

#include "LuaParser/Types/TextRange.h"
#include "LuaTokenKind.h"
#include <string>

class LuaTokenError
{
public:
	LuaTokenError(std::string_view errorMessage, TextRange range, LuaTokenKind token)
		: ErrorMessage(errorMessage),
		  ErrorRange(range),
		  MissToken(token)
	{
	}

	std::string ErrorMessage;
	TextRange ErrorRange;
	LuaTokenKind MissToken;
};
