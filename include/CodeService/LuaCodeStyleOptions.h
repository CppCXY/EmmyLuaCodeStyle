#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>

class FormatElement;
class NameStyleRuleMatcher;

enum class IndentStyle
{
	Tab,
	Space,
};

std::string GetIndentStyleName(IndentStyle style);

class LuaCodeStyleOptions
{
public:
	LuaCodeStyleOptions();

	/*
	 * 缩进风格
	 */
	IndentStyle indent_style = IndentStyle::Space;

	/*
	 * 缩进的空白数
	 */
	int indent_size = 4;

	/*
	 * tab的width
	 */
	int tab_width = 8;

	/*
	 * 延续行缩进
	 */
	int continuation_indent_size = 4;

	/*
	 * 调用参数对齐到第一个参数，经过实际体验这种风格在vscode上会因为垂直对齐线的标注而显得极为难看
	 * 
	 */
	bool align_call_args = false;

	/*
	 * 函数调用的括号和参数之间保持一个空格
	 *
	 */
	bool keep_one_space_between_call_args_and_parentheses = false;

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
	 * 设定namedef 和 他的attribute之间是否间距一个空格
	 */
	bool keep_one_space_between_namedef_and_attribute = false;

	/*
	 * 标签无缩进
	 */
	bool label_no_indent = false;

	/*
	 * do语句块内可以无缩进
	 */
	bool do_statement_no_indent = false;

	/*
	 * 表内每一个表项对齐到第一个表项
	 */
	bool align_table_field_to_first_field = true;

	bool continuous_assign_statement_align_to_equal_sign = true;

	bool continuous_assign_table_field_align_to_equal_sign = true;

	std::string end_of_line = "\r\n";

	bool insert_final_newline = true;

	std::shared_ptr<FormatElement> keep_line_after_if_statement;
	std::shared_ptr<FormatElement> keep_line_after_do_statement;
	std::shared_ptr<FormatElement> keep_line_after_while_statement;
	std::shared_ptr<FormatElement> keep_line_after_repeat_statement;
	std::shared_ptr<FormatElement> keep_line_after_for_statement;
	std::shared_ptr<FormatElement> keep_line_after_local_or_assign_statement;
	std::shared_ptr<FormatElement> keep_line_after_function_define_statement;


	// 以下是代码诊断选项，诊断选项不一定会作为格式化内容
	bool enable_check_codestyle = true;
	/*
	 * 最大行宽
	 */
	int max_line_length = 120;

	// 命名风格检查
	bool enable_name_style_check = false;
	std::shared_ptr<NameStyleRuleMatcher> local_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> function_param_name_style;
	std::shared_ptr<NameStyleRuleMatcher> function_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> local_function_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> table_field_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> global_variable_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> module_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> require_module_name_style;
	std::shared_ptr<NameStyleRuleMatcher> class_name_define_style;


};
