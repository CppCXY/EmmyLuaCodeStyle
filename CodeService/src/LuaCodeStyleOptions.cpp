#include "CodeService/LuaCodeStyleOptions.h"
#include <filesystem>
#include <regex>
#include "CodeService/FormatElement/MinLineElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"

std::shared_ptr<LuaCodeStyleOptions> LuaCodeStyleOptions::ParseFromEditorConfig(
	std::shared_ptr<LuaEditorConfig> editorConfig)
{
	auto options = std::make_shared<LuaCodeStyleOptions>();
	if (editorConfig->Exist("indent_style"))
	{
		if (editorConfig->Get("indent_style") == "space")
		{
			options->indent_style = IndentStyle::Space;
		}
		else if (editorConfig->Get("indent_style") == "tab")
		{
			options->indent_style = IndentStyle::Tab;
		}
	}

	if (editorConfig->Exist("indent_size"))
	{
		options->indent_size = std::stoi(editorConfig->Get("indent_size"));
	}

	if (editorConfig->Exist("tab_width"))
	{
		options->tab_width = std::stoi(editorConfig->Get("tab_width"));
	}


	if (editorConfig->Exist("continuation_indent_size"))
	{
		options->continuation_indent_size = std::stoi(editorConfig->Get("continuation_indent_size"));
	}

	if (editorConfig->Exist("align_call_args"))
	{
		options->align_call_args = editorConfig->Get("align_call_args") == "true";
	}

	if (editorConfig->Exist("keep_one_space_between_call_args_and_bracket"))
	{
		options->keep_one_space_between_call_args_and_bracket =
			editorConfig->Get("keep_one_space_between_call_args_and_bracket") == "true";
	}

	if (editorConfig->Exist("keep_one_space_between_table_and_bracket"))
	{
		options->keep_one_space_between_table_and_bracket =
			editorConfig->Get("keep_one_space_between_table_and_bracket") == "true";
	}

	if (editorConfig->Exist("align_table_field_to_first_field"))
	{
		options->align_table_field_to_first_field = editorConfig->Get("align_table_field_to_first_field") == "true";
	}

	if (editorConfig->Exist("continuous_assign_statement_align_to_equal_sign"))
	{
		options->continuous_assign_statement_align_to_equal_sign =
			editorConfig->Get("continuous_assign_statement_align_to_equal_sign") == "true";
	}

	if (editorConfig->Exist("continuous_assign_table_field_align_to_equal_sign"))
	{
		options->continuous_assign_table_field_align_to_equal_sign =
			editorConfig->Get("continuous_assign_table_field_align_to_equal_sign") == "true";
	}

	if (editorConfig->Exist("end_of_line"))
	{
		auto lineSeparatorSymbol = editorConfig->Get("end_of_line");
		if (lineSeparatorSymbol == "crlf")
		{
			options->end_of_line = "\r\n";
		}
		else if (lineSeparatorSymbol == "lf")
		{
			options->end_of_line = "\n";
		}
	}

	if (editorConfig->Exist("max_line_length"))
	{
		options->max_line_length = std::stoi(editorConfig->Get("max_line_length"));
	}

	if (editorConfig->Exist("enable_check_codestyle"))
	{
		options->enable_check_codestyle = editorConfig->Get("enable_check_codestyle") == "true";
	}

	std::vector<std::pair<std::string, std::shared_ptr<FormatElement>&>> fieldList = {
		{"keep_line_after_if_statement", options->keep_line_after_if_statement},
		{"keep_line_after_do_statement", options->keep_line_after_do_statement},
		{"keep_line_after_while_statement", options->keep_line_after_while_statement},
		{"keep_line_after_repeat_statement", options->keep_line_after_repeat_statement},
		{"keep_line_after_for_statement", options->keep_line_after_for_statement},
		{"keep_line_after_local_or_assign_statement", options->keep_line_after_local_or_assign_statement},
		{"keep_line_after_function_define_statement", options->keep_line_after_function_define_statement}
	};
	std::regex minLineRegex = std::regex(R"(minLine:\s*(\d+))");
	std::regex keepLineRegex = std::regex(R"(keepLine:\s*(\d+))");
	for (auto& keepLineOption : fieldList)
	{
		if (editorConfig->Exist(keepLineOption.first))
		{
			std::string value = editorConfig->Get(keepLineOption.first);
			if (value == "keepLine")
			{
				keepLineOption.second = std::make_shared<KeepLineElement>();
				continue;
			}
			std::smatch m;

			if (std::regex_search(value, m, minLineRegex))
			{
				keepLineOption.second = std::make_shared<MinLineElement>(std::stoi(m.str(1)));
				continue;
			}

			if (std::regex_search(value, m, keepLineRegex))
			{
				keepLineOption.second = std::make_shared<KeepLineElement>(std::stoi(m.str(1)));
			}
		}
	}
	return options;
}

LuaCodeStyleOptions::LuaCodeStyleOptions()
	:
	keep_line_after_if_statement(std::make_shared<MinLineElement>(1)),
	keep_line_after_do_statement(std::make_shared<MinLineElement>(1)),
	keep_line_after_while_statement(std::make_shared<MinLineElement>(1)),
	keep_line_after_repeat_statement(std::make_shared<MinLineElement>(1)),
	keep_line_after_for_statement(std::make_shared<MinLineElement>(1)),
	keep_line_after_local_or_assign_statement(std::make_shared<KeepLineElement>()),
	keep_line_after_function_define_statement(std::make_shared<KeepLineElement>(1))
{
}
