#pragma once

#include <vector>
#include <string>
#include <string_view>
#include "LuaTokenType.h"
#include <map>
#include <set>

class LuaCustomParser
{
public:
	LuaTokenType Lex(std::string_view source, std::size_t start, std::size_t& consumeSize);

	void SetTokens(LuaTokenType type, const std::vector<std::string>& customTokens);

	bool IsSupportCustomTokens() const;
private:
	std::map<std::string, LuaTokenType, std::less<>> _customTokens;
};
