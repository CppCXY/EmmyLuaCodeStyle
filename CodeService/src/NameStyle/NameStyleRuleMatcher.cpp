#include "CodeService/NameStyle/NameStyleRuleMatcher.h"
#include "LuaParser/LuaTokenTypeDetail.h"
#include <algorithm>


std::shared_ptr<NameStyleRuleMatcher> NameStyleRuleMatcher::ParseFrom(std::string_view rule)
{
	auto matcher = std::make_shared<NameStyleRuleMatcher>();
	matcher->ParseRule(rule);
	return matcher;
}

NameStyleRuleMatcher::NameStyleRuleMatcher()
{
}

void NameStyleRuleMatcher::Diagnosis(DiagnosisContext& ctx, std::shared_ptr<CheckElement> checkElement)
{

}

void NameStyleRuleMatcher::ParseRule(std::string_view rule)
{
	auto tokenParser = std::make_shared<LuaTokenParser>(std::string(rule));

	while(tokenParser->Current().TokenType != TK_EOS)
	{
		if(tokenParser->Current().TokenType == TK_NAME)
		{
			auto& lookAhead = tokenParser->LookAhead();
			if(lookAhead.TokenType == '(')
			{








			}
			else if(lookAhead.TokenType == TK_EOS || lookAhead.TokenType == '|')
			{
				auto currentText = tokenParser->Current().Text;
				if(currentText == "snake_case")
				{
					_rulers.push_back(SnakeCase);
				}
				else if(currentText == "camel_case")
				{
					_rulers.push_back(CamelCase);
				}
				else if(currentText == "pascal_case")
				{
					_rulers.push_back(PascalCase);
				}
			}

			if(lookAhead.TokenType == '|')
			{
				tokenParser->Next();
			}

		}
		else // Óï·¨´íÎó
		{
			break;
		}

		next:
		tokenParser->Next();
	}
}

bool NameStyleRuleMatcher::SnakeCase(std::shared_ptr<CheckElement> checkElement)
{
}

bool NameStyleRuleMatcher::CamelCase(std::shared_ptr<CheckElement> checkElement)
{
}

bool NameStyleRuleMatcher::PascalCase(std::shared_ptr<CheckElement> checkElement)
{
}

bool NameStyleRuleMatcher::Same(std::shared_ptr<CheckElement> checkElement, std::vector<std::string>& param)
{
}
