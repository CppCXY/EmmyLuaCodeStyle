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
	using MatchRuler = std::function<bool(std::shared_ptr<CheckElement>)>;

	static std::shared_ptr<NameStyleRuleMatcher> ParseFrom(std::string_view rule);

	NameStyleRuleMatcher();

	void Diagnosis(DiagnosisContext& ctx, std::shared_ptr<CheckElement> checkElement);

	void ParseRule(std::string_view rule);
private:
	static bool SnakeCase(std::shared_ptr<CheckElement> checkElement);
	static bool CamelCase(std::shared_ptr<CheckElement> checkElement);
	static bool PascalCase(std::shared_ptr<CheckElement> checkElement);

	static bool Same(std::shared_ptr<CheckElement> checkElement, std::vector<std::string>& param);

	std::vector<MatchRuler> _rulers;
};
