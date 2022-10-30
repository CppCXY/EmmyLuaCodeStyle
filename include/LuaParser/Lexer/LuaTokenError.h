#pragma once

#include <string>
#include "LuaParser/Types/TextRange.h"
#include "LuaTokenKind.h"

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
