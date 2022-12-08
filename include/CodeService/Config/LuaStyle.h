#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include "LuaStyleEnum.h"
#include "LuaParser/Types/EndOfLineType.h"

class LuaStyle {
public:
    LuaStyle() = default;
    // [basic]
    /*
     * 缩进风格
     */
    IndentStyle indent_style = IndentStyle::Space;

    /*
     * 缩进的空白数
     */
    std::size_t indent_size = 4;

    /*
     * tab的width
     */
    std::size_t tab_width = 4;

    QuoteStyle quote_style = QuoteStyle::None;

    TableSeparatorStyle table_separator_style = TableSeparatorStyle::Comma;

    std::size_t max_line_length = 120;
#ifndef _WINDOWS
    EndOfLine end_of_line = EndOfLine::LF;
#else
    EndOfLine end_of_line = EndOfLine::CRLF;
#endif

    TrailingTableSeparator trailing_table_separator = TrailingTableSeparator::Always;

    CallArgParentheses call_arg_parentheses = CallArgParentheses::Keep;

	std::size_t continuation_indent = 4;

	bool detect_end_of_line = true;

	bool insert_final_newline = true;

    // [Space]
    bool space_around_table_field_list = true;

    bool space_before_attribute = true;

    bool space_before_function_open_parenthesis = false;

    bool space_inside_function_call_parentheses = false;

    bool space_inside_function_param_list_parentheses = false;

    bool space_before_open_square_bracket = false;

    bool space_inside_square_brackets = false;

    // like t[#t+1] = 1
    bool space_around_table_append_operator = false;

    bool ignore_spaces_inside_function_call = false;

    std::size_t space_before_inline_comment = 1;

    // [Align]
    AlignCallArgs align_call_args = AlignCallArgs::Normal;

    bool align_function_params = true;

    // 感觉没啥用,就不实现了
    // bool align_chained_expression_statement = false;

    bool align_continuous_assign_statement_to_equal = true;

    bool align_continuous_rect_table_field_to_equal = true;

    // [Indent]

	/*
	 * if语句的条件可以无延续行缩进
	 */
	bool never_indent_before_if_condition = false;

	bool never_indent_comment_on_if_branch = false;

//
//	int max_continuous_line_distance = 1;
//
//	bool if_condition_align_with_each_other = false;
//
//	bool long_chain_expression_allow_one_space_after_colon = false;
//
//	bool remove_expression_list_finish_comma = false;
//
//	bool remove_empty_header_and_footer_lines_in_function = false;
    // [preference]

//
//	std::shared_ptr<FormatElement> keep_line_after_if_statement;
//	std::shared_ptr<FormatElement> keep_line_after_do_statement;
//	std::shared_ptr<FormatElement> keep_line_after_while_statement;
//	std::shared_ptr<FormatElement> keep_line_after_repeat_statement;
//	std::shared_ptr<FormatElement> keep_line_after_for_statement;
//	std::shared_ptr<FormatElement> keep_line_after_local_or_assign_statement;
//	std::shared_ptr<FormatElement> keep_line_after_function_define_statement;
//	std::shared_ptr<FormatElement> keep_line_after_expression_statement;
//
//
    bool enable_check_code_style = true;
//
//	// 命名风格检查
//	bool enable_name_style_check = false;
//	std::shared_ptr<NameStyleRuleMatcher> local_name_define_style;
//	std::shared_ptr<NameStyleRuleMatcher> function_param_name_style;
//	std::shared_ptr<NameStyleRuleMatcher> function_name_define_style;
//	std::shared_ptr<NameStyleRuleMatcher> local_function_name_define_style;
//	std::shared_ptr<NameStyleRuleMatcher> table_field_name_define_style;
//	std::shared_ptr<NameStyleRuleMatcher> global_variable_name_define_style;
//	std::shared_ptr<NameStyleRuleMatcher> module_name_define_style;
//	std::shared_ptr<NameStyleRuleMatcher> require_module_name_style;
//	std::shared_ptr<NameStyleRuleMatcher> class_name_define_style;
};
