#pragma once

#include <exception>
#include <string_view>

class LuaParserException : std::exception
{
public:
	LuaParserException(std::string_view message)
		: exception(message.data())
	{
	}
};
