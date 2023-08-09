#pragma once

#include "LuaParser/Types/EndOfLineType.h"
#include "LuaStyleEnum.h"
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class LuaStyle {
public:
    LuaStyle() = default;

    void Parse(std::map<std::string, std::string, std::less<>> &configMap);
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

    std::size_t continuation_indent = 4;

    TableSeparatorStyle table_separator_style = TableSeparatorStyle::Comma;

    std::size_t max_line_length = 120;
#ifndef _WINDOWS
    EndOfLine end_of_line = EndOfLine::LF;
#else
    EndOfLine end_of_line = EndOfLine::CRLF;
#endif

    TrailingTableSeparator trailing_table_separator = TrailingTableSeparator::Keep;

    CallArgParentheses call_arg_parentheses = CallArgParentheses::Keep;

    bool detect_end_of_line = true;

    bool insert_final_newline = true;

    // [Space]
    bool space_around_table_field_list = true;

    bool space_before_attribute = true;

    bool space_before_function_open_parenthesis = false;

    bool space_before_function_call_open_parenthesis = false;

    bool space_before_closure_open_parenthesis = false;

    FunctionSingleArgSpace space_before_function_call_single_arg = FunctionSingleArgSpace::Always;

    bool space_before_open_square_bracket = false;

    bool space_inside_function_call_parentheses = false;

    bool space_inside_function_param_list_parentheses = false;

    bool space_inside_square_brackets = false;

    // like t[#t+1] = 1
    bool space_around_table_append_operator = false;

    bool ignore_spaces_inside_function_call = false;

    std::size_t space_before_inline_comment = 1;

    // [operator space]
    bool space_around_math_operator = true;

    bool space_after_comma = true;

    bool space_after_comma_in_for_statement = true;

    bool space_around_concat_operator = true;

    // [Align]
    bool align_call_args = false;

    bool align_function_params = true;

    ContinuousAlign align_continuous_assign_statement = ContinuousAlign::WhenExtraSpace;

    ContinuousAlign align_continuous_rect_table_field = ContinuousAlign::WhenExtraSpace;

    std::size_t align_continuous_line_space = 2;

    bool align_if_branch = false;

    bool align_continuous_similar_call_args = false;

    bool align_continuous_inline_comment = true;

    AlignArrayTable align_array_table = AlignArrayTable::Normal;

    AlignChainExpr align_chain_expr = AlignChainExpr::None;
    // [Indent]

    /*
	 * if语句的条件可以无延续行缩进
	 */
    bool never_indent_before_if_condition = false;

    bool never_indent_comment_on_if_branch = false;

    /*
     * if true retains indents on empty lines as if they contained some code
     * if false deletes the tab characters and spaces on empty lines 
     */
    bool keep_indents_on_empty_lines = false;

    // [line space]

    LineSpace line_space_after_if_statement = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_after_do_statement = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_after_while_statement = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_after_repeat_statement = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_after_for_statement = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_after_local_or_assign_statement = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_after_function_statement = LineSpace(LineSpaceType::Fixed, 2);

    LineSpace line_space_after_expression_statement = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_after_comment = LineSpace(LineSpaceType::Keep);

    LineSpace line_space_around_block = LineSpace(LineSpaceType::Fixed, 1);
    // [line break]
    bool break_all_list_when_line_exceed = false;

    bool auto_collapse_lines = false;

    bool break_before_braces = false;

    // [preference]
    bool ignore_space_after_colon = false;

    bool remove_call_expression_list_finish_comma = false;

    // not implement now
    bool leading_comma_style = false;

    EndStmtWithSemicolon end_statement_with_semicolon = EndStmtWithSemicolon::Keep;
};
