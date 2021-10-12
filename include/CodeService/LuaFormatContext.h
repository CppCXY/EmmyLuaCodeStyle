#pragma once
#include <string_view>

class LuaFormatContext
{
public:
	void Print(std::string_view text);
	void EnterBlockEnv()
};