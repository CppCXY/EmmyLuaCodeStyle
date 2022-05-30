#pragma once

#include <vector>
#include <string>
#include <string_view>
#include "LuaTokenType.h"
#include <map>

class LuaCustomParser
{
public:
	LuaTokenType Lex(std::string_view source, std::size_t start, std::size_t& consumeSize);

	void SetEqToken(std::vector<std::string>& customTokens);
private:

	std::map<std::string, LuaTokenType, std::less<>> _customTokens;
};
