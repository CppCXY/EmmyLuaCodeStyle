#include "CodeService/LuaLinterType.h"
#include "nlohmann/json.hpp"

BasicLinterStyle::BasicLinterStyle(EnableType enable)
	: Enable(enable)
{
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

LinterLineSpace::LinterLineSpace()
	: BasicLinterStyle(EnableType::None)
{
}

void LinterLineSpace::Deserialize(nlohmann::json json)
{
	if (json.is_object())
	{
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
			{"local_name_define_style", local_name_define_style},
			{"function_param_name_style", function_param_name_style},
			{"function_name_define_style", function_name_define_style},
			{"local_function_name_define_style", local_function_name_define_style},
			{"table_field_name_define_style", table_field_name_define_style},
			{"global_variable_name_define_style", global_variable_name_define_style},
			{"module_name_define_style", module_name_define_style},
			{"require_module_name_style", require_module_name_style},
			{"class_name_define_style", class_name_define_style},
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
