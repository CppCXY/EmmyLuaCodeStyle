//#pragma once
//
//#include <functional>
//#include <string_view>
//#include <memory>
//#include "CheckElement.h"
//
//class NameStyleRuleMatcher
//{
//public:
//	enum class NameStyleType
//	{
//		Off,
//		CamelCase,
//		PascalCase,
//		SnakeCase,
//		UpperSnakeCase,
//		Same,
//		Custom
//	};
//
//	struct NameStyleRule
//	{
//		explicit NameStyleRule(NameStyleType type);
//		NameStyleRule(NameStyleType type, std::vector<std::string> param);
//		NameStyleType Type;
//		std::vector<std::string> Param;
//	};
//
//	NameStyleRuleMatcher(std::string_view name);
//
//	void Diagnosis(DiagnosisContext& ctx, std::shared_ptr<CheckElement> checkElement);
//
//	void ParseRule(std::string_view rule);
//private:
//	static bool SnakeCase(std::shared_ptr<CheckElement> checkElement);
//	static bool UpperSnakeCase(std::shared_ptr<CheckElement> checkElement);
//	static bool CamelCase(std::shared_ptr<CheckElement> checkElement);
//	static bool PascalCase(std::shared_ptr<CheckElement> checkElement);
//
//	static bool Same(DiagnosisContext& ctx, std::shared_ptr<CheckElement> checkElement,
//	                 std::vector<std::string>& param);
//
//	static bool SameSimple(std::string_view text, std::shared_ptr<CheckElement> checkElement);
//	static bool SameSnake(std::string_view text, std::shared_ptr<CheckElement> checkElement);
//	static bool SameCamel(std::string_view text, std::shared_ptr<CheckElement> checkElement);
//	static bool SamePascal(std::string_view text, std::shared_ptr<CheckElement> checkElement);
//
//	static std::vector<std::string_view> SplitPart(std::string_view source);
//
//	std::vector<NameStyleRule> _rulers;
//	std::string _name;
//};
