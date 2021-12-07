#pragma once

#include <string_view>
#include <memory>
#include "LuaParser/LuaTokenParser.h"

class NameStyleRuleMatcher
{
public:
	static std::shared_ptr<NameStyleRuleMatcher> ParseFrom(std::string_view rule);

	NameStyleRuleMatcher();

	void SetRule(std::string_view rule);

	void ParseRule();
private:

	std::shared_ptr<LuaTokenParser> _tokenParser;
	std::string _rule;
};
