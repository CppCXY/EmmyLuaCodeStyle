#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <map>

class FormatElement;

class LuaCodeStyleOptions
{
public:
	static std::shared_ptr<LuaCodeStyleOptions> ParseFromEditorConfig(std::string_view filename);
	static std::shared_ptr<LuaCodeStyleOptions> ParseOptionsFromMap(std::map<std::string, std::string>& optionMap);
	LuaCodeStyleOptions();
	/*
	 * 我反对这种风格
	 * 但我依然实现他
	 */
	bool use_tab = false;

	/*
	 * 缩进的空白数
	 */
	int indent = 4;

	/*
	 * 延续行缩进
	 */
	int continuation_indent = 4;

	/*
	 * 调用参数对齐到第一个参数，经过实际体验这种风格在vscode上会因为垂直对齐线的标注而显得极为难看
	 * 
	 */
	bool align_call_args = false;

	/*
	 * 函数调用的括号和参数之间保持一个空格
	 *
	 */
	bool keep_one_space_between_call_args_and_bracket = false;

	/*
	 * 函数定义的参数保持对齐到第一个参数
	 * 函数的定义通常不会太长，这种默认行为是可以接受的
	 */
	bool align_function_define_params = true;

	/*
	 * 表的大括号和表项之间保持一个空格
	 * 经过实际体验这种方式比较美观
	 */
	bool keep_one_space_between_table_and_bracket = true;

	/*
	 * 表内每一个表项对齐到第一个表项
	 */
	bool align_table_field_to_first_field = true;

	bool continuous_assign_statement_align_to_equal_sign = true;

	bool continuous_assign_table_field_align_to_equal_sign = true;

	std::string line_separator = "\r\n";

	std::shared_ptr<FormatElement> keep_line_after_if_statement;
	std::shared_ptr<FormatElement> keep_line_after_do_statement;
	std::shared_ptr<FormatElement> keep_line_after_while_statement;
	std::shared_ptr<FormatElement> keep_line_after_repeat_statement;
	std::shared_ptr<FormatElement> keep_line_after_for_statement;
	std::shared_ptr<FormatElement> keep_line_after_local_or_assign_statement;
	std::shared_ptr<FormatElement> keep_line_after_function_define_statement;


	// 以下是代码诊断选项，诊断选项不一定会作为格式化内容

	/*
	 * 最大行宽
	 */
	int max_line_length = 120;

};
