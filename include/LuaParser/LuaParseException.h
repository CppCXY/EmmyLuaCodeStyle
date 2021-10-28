#pragma once

#include <exception>
#include <string_view>

class LuaParserException : std::exception
{
public:
	// ºÊ»›gcc
	LuaParserException(const char* message)
		: exception(message)
	{
	}
};
