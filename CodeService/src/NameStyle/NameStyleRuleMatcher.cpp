#include "CodeService/NameStyle/NameStyleRuleMatcher.h"
#include "LuaParser/LuaTokenTypeDetail.h"
#include <algorithm>


std::shared_ptr<NameStyleRuleMatcher> NameStyleRuleMatcher::ParseFrom(std::string_view rule)
{
	auto matcher = std::make_shared<NameStyleRuleMatcher>();
	matcher->SetRule(rule);
	matcher->ParseRule();
	return matcher;
}

NameStyleRuleMatcher::NameStyleRuleMatcher()
	: _tokenParser(nullptr)
{
}

void NameStyleRuleMatcher::SetRule(std::string_view rule)
{
	_tokenParser = std::make_shared<LuaTokenParser>(std::string(rule));
}

void NameStyleRuleMatcher::ParseRule()
{
	enum class ParseState
	{
		NamedRule,
		FunctionRule
	} state = ParseState::NamedRule;

	while (_tokenParser->Current().TokenType != TK_EOS)
	{
		auto type = _tokenParser->Current().TokenType;
		switch (state)
		{
		case ParseState::NamedRule:
			{
				if(type == TK_NAME)
				{
					
				}
				else if(type == '|')
				{
					break;
				}
				else if(type == '(')
				{
					state = ParseState::FunctionRule;

				}
				break;
			}
		case ParseState::FunctionRule:
			{
				break;
			}
		}
		_tokenParser->Next();
	}
}
