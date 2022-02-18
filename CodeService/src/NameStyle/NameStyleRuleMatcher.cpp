#include "CodeService/NameStyle/NameStyleRuleMatcher.h"
#include <algorithm>
#include "LuaParser/LuaTokenTypeDetail.h"
#include "Util/format.h"
#include "CodeService/LanguageTranslator.h"
#include "Util/StringUtil.h"

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
		case NameStyleType::UpperSnakeCase:
			{
				if (UpperSnakeCase(checkElement))
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
				if (Same(ctx, checkElement, rule.Param))
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
				if (rule.Param.size() == 1)
				{
					ruleMessage.append(format("same({})", rule.Param[0]));
				}
				else if (rule.Param.size() == 2)
				{
					ruleMessage.append(format("same({},{})", rule.Param[0], rule.Param[1]));
				}
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
	auto file = std::make_shared<LuaFile>("", std::string(rule));
	auto tokenParser = std::make_shared<LuaTokenParser>(file);
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
				tokenParser->Next();
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
				else if (currentText == "upper_snake_case")
				{
					_rulers.emplace_back(NameStyleType::UpperSnakeCase);
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
					if (source.size() > 1 && !::islower(source[index + 1]))
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

bool NameStyleRuleMatcher::Same(DiagnosisContext& ctx, std::shared_ptr<CheckElement> checkElement,
                                std::vector<std::string>& param)
{
	if (param.empty())
	{
		return true;
	}

	auto& firstParam = param.front();

	if (firstParam == "first_param")
	{
		// 可空判断太长了
		if (!checkElement->ExtraInfoNode)
		{
			return true;
		}
		if (checkElement->ExtraInfoNode->GetType() != LuaAstNodeType::CallArgList)
		{
			return true;
		}

		auto firstParamNode = checkElement->ExtraInfoNode->FindFirstOf(LuaAstNodeType::StringLiteralExpression);

		if (!firstParamNode)
		{
			auto expressionList = checkElement->ExtraInfoNode->FindFirstOf(LuaAstNodeType::ExpressionList);
			if (!expressionList)
			{
				return true;
			}
			auto expressionNode = expressionList->FindFirstOf(LuaAstNodeType::Expression);
			if (!expressionNode)
			{
				return true;
			}
			firstParamNode = expressionNode->FindFirstOf(LuaAstNodeType::StringLiteralExpression);
			if (!firstParamNode)
			{
				return true;
			}
		}

		std::string_view firstText = firstParamNode->GetText();

		if (firstText.size() <= 2)
		{
			return true;
		}

		firstText = firstText.substr(1, firstText.size() - 2);
		if (param.size() == 2)
		{
			auto& secondParam = param[1];

			if (secondParam == "snake_case")
			{
				return SameSnake(firstText, checkElement);
			}
			else if (secondParam == "camel_case")
			{
				return SameCamel(firstText, checkElement);
			}
			else if (secondParam == "pascal_case")
			{
				return SamePascal(firstText, checkElement);
			}
		}
		else
		{
			return SameSimple(firstText, checkElement);
		}
	}
	else if (firstParam == "filename")
	{
		auto filename = ctx.GetParser()->GetFilename();

		if (param.size() == 2)
		{
			auto& secondParam = param[1];

			if (secondParam == "snake_case")
			{
				return SameSnake(filename, checkElement);
			}
			else if (secondParam == "camel_case")
			{
				return SameCamel(filename, checkElement);
			}
			else if (secondParam == "pascal_case")
			{
				return SamePascal(filename, checkElement);
			}
		}
		else
		{
			return SameSimple(filename, checkElement);
		}
	}
	else if (firstParam.size() > 2 && (StringUtil::StartWith(firstParam, "\'") || StringUtil::StartWith(
		firstParam, "\"")))
	{
		auto name = firstParam.substr(1, firstParam.size() - 2);
		return checkElement->Node->GetText() == name;
	}

	return true;
}

bool NameStyleRuleMatcher::SameSimple(std::string_view text, std::shared_ptr<CheckElement> checkElement)
{
	auto checkText = checkElement->Node->GetText();
	return checkText == text;
}

bool NameStyleRuleMatcher::SameSnake(std::string_view text, std::shared_ptr<CheckElement> checkElement)
{
	if (!SnakeCase(checkElement))
	{
		return false;
	}

	auto checkText = checkElement->Node->GetText();
	auto textParts = SplitPart(text);
	auto checkParts = SplitPart(checkText);

	if (checkParts.size() > textParts.size())
	{
		return false;
	}

	for (std::size_t i = 1; i <= checkParts.size(); i++)
	{
		if (checkParts[checkParts.size() - i] != textParts[textParts.size() - i])
		{
			return false;
		}
	}

	return true;
}

bool NameStyleRuleMatcher::SameCamel(std::string_view text, std::shared_ptr<CheckElement> checkElement)
{
	if (!CamelCase(checkElement))
	{
		return false;
	}

	auto checkText = checkElement->Node->GetText();
	auto textParts = SplitPart(text);


	for (auto it = textParts.rbegin(); it != textParts.rend(); ++it)
	{
		if (StringUtil::EndWith(checkText, *it))
		{
			if (checkText.size() == it->size())
			{
				return true;
			}
			checkText = checkText.substr(0, checkText.size() - it->size());
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool NameStyleRuleMatcher::SamePascal(std::string_view text, std::shared_ptr<CheckElement> checkElement)
{
	if (!PascalCase(checkElement))
	{
		return false;
	}

	auto checkText = checkElement->Node->GetText();
	auto textParts = SplitPart(text);


	for (auto it = textParts.rbegin(); it != textParts.rend(); --it)
	{
		if (StringUtil::EndWith(checkText, *it))
		{
			if (checkText.size() == it->size())
			{
				return true;
			}
			checkText = checkText.substr(0, checkText.size() - it->size());
		}
		else
		{
			return false;
		}
	}

	return true;
}

std::vector<std::string_view> NameStyleRuleMatcher::SplitPart(std::string_view source)
{
	std::vector<std::string_view> result;
	std::size_t lastIndex = 0;
	for (std::size_t index = 0; index < source.size(); index++)
	{
		char ch = source[index];

		if ((ch == '.' || ch == '_' || ch == '-' || ch == '/' || ch == '\\') && index != 0)
		{
			result.push_back(source.substr(lastIndex, index - lastIndex));
			lastIndex = index + 1;
		}
	}

	if (lastIndex < source.size())
	{
		result.push_back(source.substr(lastIndex, source.size() - lastIndex));
	}

	return result;
}
