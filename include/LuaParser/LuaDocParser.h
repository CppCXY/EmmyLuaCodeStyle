#pragma once

#include <string_view>

class LuaDocParser
{
public:
	LuaDocParser(std::string_view doc);
	~LuaDocParser();

private:
	std::string_view _source;

};
