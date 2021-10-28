#pragma once

#include <exception>
#include <string_view>

class LuaParserException : std::exception
{
public:
	// ºÊ»›gcc
	LuaParserException(std::string_view message)
		: exception(std::string(message).c_str())
	{
	}
};
