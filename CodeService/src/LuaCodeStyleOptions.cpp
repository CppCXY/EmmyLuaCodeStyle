#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeService/FormatElement/MinLineElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"
#include "CodeService/NameStyle/NameStyleRuleMatcher.h"

std::string GetIndentStyleName(IndentStyle style)
{
	switch (style)
	{
	case IndentStyle::Tab:
		{
			return "tab";
		}
	case IndentStyle::Space:
		{
			return "space";
		}
	}
	return "unknown";
}

LuaCodeStyleOptions::LuaCodeStyleOptions()
	:
	keep_line_after_if_statement(std::make_shared<MinLineElement>(0)),
	keep_line_after_do_statement(std::make_shared<MinLineElement>(0)),
	keep_line_after_while_statement(std::make_shared<MinLineElement>(0)),
	keep_line_after_repeat_statement(std::make_shared<MinLineElement>(0)),
	keep_line_after_for_statement(std::make_shared<MinLineElement>(0)),
	keep_line_after_local_or_assign_statement(std::make_shared<KeepLineElement>()),
	keep_line_after_function_define_statement(std::make_shared<KeepLineElement>(1)),

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
