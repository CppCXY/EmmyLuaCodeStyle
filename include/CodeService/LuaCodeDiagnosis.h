#pragma once

#include "LuaParser/TextRange.h"

class LuaCodeDiagnosis
{
public:
	LuaCodeDiagnosis(TextRange range, std::string_view message)
		: Range(range),
		  Message(message)
	{
	}

	TextRange Range;
	std::string Message;
};
