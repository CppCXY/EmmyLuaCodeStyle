#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include "LuaCodeStyleEnum.h"
#include "LuaParser/EndOfLineType.h"

class FormatElement;
class NameStyleRuleMatcher;

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
	int tab_width = 4;

	QuoteStyle quote_style = QuoteStyle::None;

	CallArgParentheses call_arg_parentheses = CallArgParentheses::Keep;
	/*
	 * 延续行缩进
	 */
	int continuation_indent_size = 4;

	int statement_inline_comment_space = 1;

	bool local_assign_continuation_align_to_first_expression = false;

	/*
	 * no opened
	 */
	bool table_field_continuation_align_to_first_sub_expression = false;

	/*
	 * 调用参数对齐到第一个参数
	 */
	AlignCallArgs align_call_args = AlignCallArgs::False;

	/*
	 * 函数定义的参数保持对齐到第一个参数
	 * 函数的定义通常不会太长，这种默认行为是可以接受的
	 */
	bool align_function_define_params = true;

	bool align_chained_expression_statement = false;
	/*
	 * 表的大括号和表项之间保持一个空格
	 * 经过实际体验这种方式比较美观
	 */
	bool keep_one_space_between_table_and_bracket = true;

	/*
	 * 设定namedef 和 他的attribute之间是否间距一个空格
	 */
	bool keep_one_space_between_namedef_and_attribute = true;

	bool space_before_function_open_parenthesis = false;

	bool space_before_open_square_bracket = false;

	/*
	 * 标签无缩进
	 */
	bool label_no_indent = false;

	/*
	 * do语句块内可以无缩进
	 */
	bool do_statement_no_indent = false;

	/*
	 * if语句的条件可以无延续行缩进
	 */
	bool if_condition_no_continuation_indent = false;

	bool if_branch_comments_after_block_no_indent = false;
	/*
	 * 表内每一个表项对齐到第一个表项
	 */
	bool align_table_field_to_first_field = false;

	/*
	 * 如何认定连续行,相距超过该选项的值则认为不连续
	 */
	int max_continuous_line_distance = 1;

	bool continuous_assign_statement_align_to_equal_sign = true;

	bool continuous_assign_table_field_align_to_equal_sign = true;

	bool if_condition_align_with_each_other = false;

	bool table_append_expression_no_space = false;

	bool long_chain_expression_allow_one_space_after_colon = false;

	bool remove_expression_list_finish_comma = false;

	bool remove_empty_header_and_footer_lines_in_function = false;
#ifndef _WINDOWS
	EndOfLine end_of_line = EndOfLine::LF;
#else
	EndOfLine end_of_line = EndOfLine::CRLF;
#endif
	bool detect_end_of_line = false;

	bool insert_final_newline = true;

	std::shared_ptr<FormatElement> keep_line_after_if_statement;
	std::shared_ptr<FormatElement> keep_line_after_do_statement;
	std::shared_ptr<FormatElement> keep_line_after_while_statement;
	std::shared_ptr<FormatElement> keep_line_after_repeat_statement;
	std::shared_ptr<FormatElement> keep_line_after_for_statement;
	std::shared_ptr<FormatElement> keep_line_after_local_or_assign_statement;
	std::shared_ptr<FormatElement> keep_line_after_function_define_statement;
	std::shared_ptr<FormatElement> keep_line_after_expression_statement;


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
