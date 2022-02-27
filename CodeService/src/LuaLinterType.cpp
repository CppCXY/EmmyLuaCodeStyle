#include "CodeService/LuaLinterType.h"
#include "nlohmann/json.hpp"
#include <regex>

#include "CodeService/FormatElement/KeepElement.h"
#include "CodeService/FormatElement/MinLineElement.h"

BasicLinterStyle::BasicLinterStyle(EnableType enable)
	: Enable(enable)
{
}

BasicLinterStyle::operator bool()
{
	return Enable == EnableType::Enable;
}

LinterMaxLineLength::LinterMaxLineLength()
	: BasicLinterStyle(EnableType::None),
	  Value(0)
{
}

void LinterMaxLineLength::Deserialize(nlohmann::json json)
{
	if (json.is_number_integer())
	{
		Value = json;
		Enable = EnableType::Enable;
	}
}

LinterNoIndentStyle::LinterNoIndentStyle()
	: BasicLinterStyle(EnableType::None),
	  DoStatement(false),
	  Label(false)
{
}

void LinterNoIndentStyle::Deserialize(nlohmann::json json)
{
	if (json.is_array())
	{
		for (auto& j : json)
		{
			if (j.is_string())
			{
				std::string value = j;

				if (value == "do_statement")
				{
					DoStatement = true;
				}
				else if (value == "label")
				{
					Label = true;
				}
			}
		}
		Enable = EnableType::Enable;
	}
}

LinterIndentStyle::LinterIndentStyle()
	: BasicLinterStyle(EnableType::None),
	  Style(IndentStyle::Space),
	  IndentSize(4),
	  TabWidth(4)
{
}

void LinterIndentStyle::Deserialize(nlohmann::json json)
{
	if (json.is_object())
	{
		if (json["style"].is_string())
		{
			std::string style = json["style"];
			if (style == "space")
			{
				Style = IndentStyle::Space;
			}
			else if (style == "tab")
			{
				Style = IndentStyle::Tab;
			}
		}

		if (json["indent-size"].is_number_integer())
		{
			IndentSize = json["indent-size"];
		}

		if (json["tab-width"].is_number_integer())
		{
			TabWidth = json["tab-width"];
		}

		if (json["no-indent"].is_array())
		{
			NoIndent.Deserialize(json["no-indent"]);
		}

		Enable = EnableType::Enable;
	}
}

LinterQuoteStyle::LinterQuoteStyle()
	: BasicLinterStyle(EnableType::None),
	  Style(QuoteStyle::None)
{
}

void LinterQuoteStyle::Deserialize(nlohmann::json json)
{
	if (json.is_string())
	{
		std::string quoteStyle = json;
		if (quoteStyle == "double")
		{
			Style = QuoteStyle::Double;
		}
		else if (quoteStyle == "single")
		{
			Style = QuoteStyle::Single;
		}
		else if (quoteStyle == "none")
		{
			Style = QuoteStyle::None;
		}
		Enable = EnableType::Enable;
	}
}

LinterAlignStyle::LinterAlignStyle()
	: BasicLinterStyle(EnableType::None),
	  MaxContinuousLineDistance(1),
	  LocalOrAssign(AlignStyleLevel::None),
	  IfCondition(AlignStyleLevel::None),
	  TableField(AlignStyleLevel::None)
{
}

void LinterAlignStyle::Deserialize(nlohmann::json json)
{
	if (json.is_object())
	{
		std::vector<std::pair<std::string, AlignStyleLevel&>> list = {
			{"local-or-assign", LocalOrAssign},
			{"if-condition", IfCondition},
			{"table-field", TableField}
		};
		for (auto pair : list)
		{
			if (json[pair.first].is_string())
			{
				std::string level = json[pair.first];

				if (level == "allow")
				{
					pair.second = AlignStyleLevel::Allow;
				}
				else if (level == "ban")
				{
					pair.second = AlignStyleLevel::Ban;
				}
				else if (level == "none")
				{
					pair.second = AlignStyleLevel::None;
				}
				else if (level == "suggest")
				{
					pair.second = AlignStyleLevel::Suggest;
				}
			}
		}
		Enable = EnableType::Enable;
	}
}

LinterWhiteSpace::LinterWhiteSpace()
	: BasicLinterStyle(EnableType::None)
{
}

TextSpaceType LinterWhiteSpace::CalculateSpace(std::shared_ptr<LuaAstNode> node)
{
	switch (node->GetType())
	{
	case LuaAstNodeType::GeneralOperator:
	case LuaAstNodeType::KeyWord:
		{
			auto text = node->GetText();
			if (WrapperSpace.count(text))
			{
				return TextSpaceType::WrapSpace;
			}

			if (NoWrapperSpace.count(text))
			{
				return TextSpaceType::NoSpace;
			}

			if (AfterSpace.count(text))
			{
				return TextSpaceType::AfterSpace;
			}

			break;
		}
	default:
		{
			return TextSpaceType::None;
		}
	}
	return TextSpaceType::None;
}

void LinterWhiteSpace::Deserialize(nlohmann::json json)
{
}

LinterDetailLineSpace::LinterDetailLineSpace(LinterDescription description)
	: BasicLinterStyle(EnableType::None),
	  Description(description),
	  Element(nullptr)
{
}

void LinterDetailLineSpace::Deserialize(nlohmann::json json)
{
	if (json.is_string())
	{
		do
		{
			std::string value = json;
			if (value == "keep_line")
			{
				Element = std::make_shared<KeepLineElement>();
				Element->SetDescription(Description);
				break;
			}

			std::regex minLineRegex = std::regex(R"(min_line:\s*(\d+))");
			std::regex keepLineRegex = std::regex(R"(keep_line:\s*(\d+))");
			std::smatch m;

			if (std::regex_search(value, m, minLineRegex))
			{
				Element = std::make_shared<MinLineElement>(std::stoi(m.str(1)));
				Element->SetDescription(Description);
				break;
			}

			if (std::regex_search(value, m, keepLineRegex))
			{
				Element = std::make_shared<KeepLineElement>(std::stoi(m.str(1)));
				Element->SetDescription(Description);
				break;
			}

			return;
		}
		while (false);
		Enable = EnableType::Enable;
	}
}

LinterLineSpace::LinterLineSpace()
	: BasicLinterStyle(EnableType::None),
	  keep_line_after_if_statement(LinterDescription::keep_line_after_if_statement),
	  keep_line_after_do_statement(LinterDescription::keep_line_after_do_statement),
	  keep_line_after_while_statement(LinterDescription::keep_line_after_while_statement),
	  keep_line_after_repeat_statement(LinterDescription::keep_line_after_repeat_statement),
	  keep_line_after_for_statement(LinterDescription::keep_line_after_for_statement),
	  keep_line_after_local_or_assign_statement(LinterDescription::keep_line_after_local_or_assign_statement),
	  keep_line_after_function_define_statement(LinterDescription::keep_line_after_function_define_statement)
{
}

void LinterLineSpace::Deserialize(nlohmann::json json)
{
	if (json.is_object())
	{
		std::vector<std::pair<std::string, LinterDetailLineSpace&>> fieldList = {
			{"keep-line-after-if-statement", keep_line_after_if_statement},
			{"keep-line-after-do-statement", keep_line_after_do_statement},
			{"keep-line-after-while-statement", keep_line_after_while_statement},
			{"keep-line-after-repeat-statement", keep_line_after_repeat_statement},
			{"keep-line-after-for-statement", keep_line_after_for_statement},
			{"keep-line-after-local-or-assign-statement", keep_line_after_local_or_assign_statement},
			{"keep-line-after-function-define-statement", keep_line_after_function_define_statement}
		};

		for (auto& keepLineOption : fieldList)
		{
			if (!json[keepLineOption.first].is_null())
			{
				keepLineOption.second.Deserialize(json[keepLineOption.first]);
			}
		}

		Enable = EnableType::Enable;
	}
}

LinterNameStyle::LinterNameStyle()
	: BasicLinterStyle(EnableType::Enable),
	  local_name_define_style(std::make_shared<NameStyleRuleMatcher>("Local")),
	  function_param_name_style(std::make_shared<NameStyleRuleMatcher>("Parameters")),
	  function_name_define_style(std::make_shared<NameStyleRuleMatcher>("Class and module method")),
	  local_function_name_define_style(std::make_shared<NameStyleRuleMatcher>("Local method")),
	  table_field_name_define_style(std::make_shared<NameStyleRuleMatcher>("Table field")),
	  global_variable_name_define_style(std::make_shared<NameStyleRuleMatcher>("Global")),
	  module_name_define_style(std::make_shared<NameStyleRuleMatcher>("Module")),
	  require_module_name_style(std::make_shared<NameStyleRuleMatcher>("Require module")),
	  class_name_define_style(std::make_shared<NameStyleRuleMatcher>("Class"))

{
}

void LinterNameStyle::Deserialize(nlohmann::json json)
{
	if (json.is_object())
	{
		std::vector<std::pair<std::string, std::shared_ptr<NameStyleRuleMatcher>&>> styleList = {
			{"local-name", local_name_define_style},
			{"function-param", function_param_name_style},
			{"function-name", function_name_define_style},
			{"local-function", local_function_name_define_style},
			{"table-field", table_field_name_define_style},
			{"global", global_variable_name_define_style},
			{"module", module_name_define_style},
			{"require-module", require_module_name_style},
			{"class", class_name_define_style},
		};

		for (auto& styleOption : styleList)
		{
			if (json[styleOption.first].is_string())
			{
				std::string value = json[styleOption.first];
				styleOption.second->ParseRule(value);
			}
		}
		Enable = EnableType::Enable;
	}
}
