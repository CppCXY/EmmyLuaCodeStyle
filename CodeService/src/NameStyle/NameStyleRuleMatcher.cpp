#include "CodeService/NameStyle/NameStyleRuleMatcher.h"
#include <algorithm>
#include "LuaParser/LuaTokenTypeDetail.h"
#include "Util/format.h"
#include "CodeService/LanguageTranslator.h"


NameStyleRuleMatcher::NameStyleRule::NameStyleRule(NameStyleType type)
	: Type(type)
{
}

NameStyleRuleMatcher::NameStyleRule::NameStyleRule(NameStyleType type, std::vector<std::string> param)
	: Type(type),
	  Param(param)
{
}

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
	if (_rulers.empty())
	{
		return;
	}

	for (auto& rule : _rulers)
	{
		switch (rule.Type)
		{
		case NameStyleType::SnakeCase:
			{
				if (SnakeCase(checkElement))
				{
					return;
				}
				break;
			}
		case NameStyleType::CamelCase:
			{
				if (CamelCase(checkElement))
				{
					return;
				}
				break;
			}
		case NameStyleType::PascalCase:
			{
				if (PascalCase(checkElement))
				{
					return;
				}
				break;
			}
		case NameStyleType::Same:
			{
				if (Same(checkElement, rule.Param))
				{
					return;
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}

	std::string ruleMessage = "";
	for (std::size_t index = 0; index < _rulers.size(); index++)
	{
		auto& rule = _rulers[index];
		switch (rule.Type)
		{
		case NameStyleType::SnakeCase:
			{
				ruleMessage.append("snake-case");
				break;
			}
		case NameStyleType::CamelCase:
			{
				ruleMessage.append("camel-case");
				break;
			}
		case NameStyleType::PascalCase:
			{
				ruleMessage.append("pascal-case");
				break;
			}
		case NameStyleType::Same:
			{
				// if (Same(checkElement, rule.Param))
				// {
				// 	return;
				// }
				break;
			}
		default:
			{
				break;
			}
		}
		if (index != _rulers.size() - 1)
		{
			ruleMessage.append(" | ");
		}
	}

	ctx.PushDiagnosis(format(LText("{} not match name rule {}"), checkElement->Node->GetText(), ruleMessage),
	                  checkElement->Node->GetTextRange());
}

void NameStyleRuleMatcher::ParseRule(std::string_view rule)
{
	auto tokenParser = std::make_shared<LuaTokenParser>(std::string(rule));
	tokenParser->Parse();

	while (tokenParser->Current().TokenType != TK_EOS)
	{
		if (tokenParser->Current().TokenType == TK_NAME)
		{
			auto& lookAhead = tokenParser->LookAhead();
			if (lookAhead.TokenType == '(')
			{
				NameStyleType type = NameStyleType::Same;
				auto functionName = tokenParser->Current().Text;
				if (functionName != "same")
				{
					type = NameStyleType::Custom;
				}
				std::vector<std::string> param;
				tokenParser->Next();
				while (tokenParser->Current().TokenType != ')' && tokenParser->Current().TokenType != TK_EOS)
				{
					if (tokenParser->Current().TokenType == TK_NAME || tokenParser->Current().TokenType == TK_STRING)
					{
						param.push_back(std::string(tokenParser->Current().Text));
					}
					tokenParser->Next();
				}

				_rulers.emplace_back(type, param);
			}
			else if (lookAhead.TokenType == TK_EOS || lookAhead.TokenType == '|')
			{
				auto currentText = tokenParser->Current().Text;
				if (currentText == "snake_case")
				{
					_rulers.emplace_back(NameStyleType::SnakeCase);
				}
				else if (currentText == "camel_case")
				{
					_rulers.emplace_back(NameStyleType::CamelCase);
				}
				else if (currentText == "pascal_case")
				{
					_rulers.emplace_back(NameStyleType::PascalCase);
				}
			}

			if (lookAhead.TokenType == '|')
			{
				tokenParser->Next();
			}
		}
		else // 语法错误
		{
			break;
		}

		tokenParser->Next();
	}
}

bool NameStyleRuleMatcher::SnakeCase(std::shared_ptr<CheckElement> checkElement)
{
	auto source = checkElement->Node->GetText();
	for (std::size_t index = 0; index != source.size(); index++)
	{
		char ch = source[index];
		if (ch < 0)
		{
			return false;
		}

		if (!::islower(ch))
		{
			if (ch == '_')
			{
				// 不允许双下划线
				if ((index < source.size() - 1) && source[index + 1] == '_')
				{
					return false;
				}
			}
			else if (!::isdigit(ch))
			{
				return false;
			}
		}
	}
	return true;
}

bool NameStyleRuleMatcher::UpperSnakeCase(std::shared_ptr<CheckElement> checkElement)
{
	auto source = checkElement->Node->GetText();
	bool lowerCase = false;
	for (std::size_t index = 0; index != source.size(); index++)
	{
		char ch = source[index];
		if (ch < 0)
		{
			return false;
		}
		
		else if (!::isupper(ch))
		{
			if (ch == '_')
			{
				// 不允许双下划线
				if ((index < source.size() - 1) && source[index + 1] == '_')
				{
					return false;
				}
			}
			else if (!::isdigit(ch))
			{
				return false;
			}
		}
	}
	return true;
}

bool NameStyleRuleMatcher::CamelCase(std::shared_ptr<CheckElement> checkElement)
{
	auto source = checkElement->Node->GetText();
	for (std::size_t index = 0; index != source.size(); index++)
	{
		char ch = source[index];
		if (ch < 0)
		{
			return false;
		}
		if (index == 0)
		{
			// 首字母可以小写，也可以单下划线开头
			if (!::islower(ch))
			{
				if (ch == '_')
				{
					if (source.size() > 1 && !::islower(ch))
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
		}
		else if (!::isalnum(ch))
		{
			return false;
		}
	}
	return true;
}

bool NameStyleRuleMatcher::PascalCase(std::shared_ptr<CheckElement> checkElement)
{
	auto source = checkElement->Node->GetText();
	for (std::size_t index = 0; index != source.size(); index++)
	{
		char ch = source[index];
		if (ch < 0)
		{
			return false;
		}

		if (index == 0)
		{
			// 首字母必须大写
			if (!::isupper(ch))
			{
				// _ 亚元不受命名限制
				if (source.size() == 1 && ch == '_')
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		// 我又没办法分词简单处理下
		else if (!::isalnum(ch))
		{
			return false;
		}
	}
	return true;
}

bool NameStyleRuleMatcher::Same(std::shared_ptr<CheckElement> checkElement, std::vector<std::string>& param)
{
	return true;
}
