#pragma once

#include <functional>
#include <string_view>
#include <memory>
#include "CheckElement.h"
#include "CodeService/FormatElement/DiagnosisContext.h"
#include "LuaParser/LuaTokenParser.h"
#include "LuaParser/LuaAstNode.h"

class NameStyleRuleMatcher
{
public:
	enum class NameStyleType
	{
		Off,
		CamelCase,
		PascalCase,
		SnakeCase,
		Same,
		Custom
	};

	struct NameStyleRule
	{
		explicit NameStyleRule(NameStyleType type);
		NameStyleRule(NameStyleType type, std::vector<std::string> param);
		NameStyleType Type;
		std::vector<std::string> Param;
	};

	static std::shared_ptr<NameStyleRuleMatcher> ParseFrom(std::string_view rule);

	NameStyleRuleMatcher();

	void Diagnosis(DiagnosisContext& ctx, std::shared_ptr<CheckElement> checkElement);

	void ParseRule(std::string_view rule);
private:
	static bool SnakeCase(std::shared_ptr<CheckElement> checkElement);
	static bool UpperSnakeCase(std::shared_ptr<CheckElement> checkElement);
	static bool CamelCase(std::shared_ptr<CheckElement> checkElement);
	static bool PascalCase(std::shared_ptr<CheckElement> checkElement);

	static bool Same(std::shared_ptr<CheckElement> checkElement, std::vector<std::string>& param);

	std::vector<NameStyleRule> _rulers;
};
