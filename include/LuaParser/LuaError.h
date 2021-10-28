#pragma once

#include <string>
#include "TextRange.h"


class LuaError
{
public:
	LuaError(std::string_view errorMessage, TextRange range)
		: ErrorMessage(errorMessage),
		  ErrorRange(range)
	{
	}

	std::string ErrorMessage;
	TextRange ErrorRange;
};
