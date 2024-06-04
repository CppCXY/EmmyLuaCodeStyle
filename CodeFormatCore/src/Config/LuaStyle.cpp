#include "CodeFormatCore/Config/LuaStyle.h"
#include "FunctionOption.h"

#include <functional>
#include <map>

bool IsNumber(std::string_view source) {
    if (source.empty()) {
        return false;
    }
    for (auto c: source) {
        if (c > '9' || c < '0') {
            return false;
        }
    }
    return true;
}

std::string GetOption(const std::map<std::string, std::string, std::less<>> &configMap, const std::string &key) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        return it->second;
    }
    return "";
}

#define IF_EXIST(op) if (auto value = GetOption(configMap, #op); !value.empty())

#define BOOL_OPTION(op)                                   \
    {                                                     \
        auto value = GetOption(configMap, #op);           \
        if (value == "true" || value == "always") {       \
            op = true;                                    \
        } else if (value == "false" || value == "none") { \
            op = false;                                   \
        }                                                 \
    }

#define NUMBER_OPTION(op)                        \
    {                                            \
        auto value = GetOption(configMap, #op);  \
        if (!value.empty() && IsNumber(value)) { \
            op = std::stoi(value);               \
        }                                        \
    }

void LuaStyle::Parse(std::map<std::string, std::string, std::less<>> &configMap) {
    IF_EXIST(indent_style) {
        if (value == "tab") {
            indent_style = IndentStyle::Tab;
        } else if (value == "space") {
            indent_style = IndentStyle::Space;
        }
    }

    NUMBER_OPTION(indent_size)

    NUMBER_OPTION(tab_width)

    IF_EXIST(quote_style) {
        if (value == "single") {
            quote_style = QuoteStyle::Single;
        } else if (value == "double") {
            quote_style = QuoteStyle::Double;
        }
    }

    // for editorconfig Domain-Specific Properties
    IF_EXIST(qoute_type) {
        if (value == "single") {
            quote_style = QuoteStyle::Single;
        } else if (value == "double") {
            quote_style = QuoteStyle::Double;
        }
    }

    IF_EXIST(table_separator_style) {
        if (value == "none") {
            table_separator_style = TableSeparatorStyle::None;
        } else if (value == "comma") {
            table_separator_style = TableSeparatorStyle::Comma;
        } else if (value == "semicolon") {
            table_separator_style = TableSeparatorStyle::Semicolon;
        } else if (value == "only_kv_colon") {
            table_separator_style = TableSeparatorStyle::OnlyKVColon;
        }
    }

    IF_EXIST(max_line_length) {
        if (IsNumber(value)) {
            max_line_length = std::stoi(value);
        } else if (value == "unset") {
            max_line_length = 998;
        }
    }

    IF_EXIST(end_of_line) {
        if (value == "crlf") {
            end_of_line = EndOfLine::CRLF;
        } else if (value == "lf") {
            end_of_line = EndOfLine::LF;
        } else if (value == "cr") {
            end_of_line = EndOfLine::CR;
        } else if (value == "auto" || value == "unset") {
#ifndef _WINDOWS
            end_of_line = EndOfLine::LF;
#else
            end_of_line = EndOfLine::CRLF;
#endif
        }
    }

    IF_EXIST(trailing_table_separator) {
        if (value == "keep") {
            trailing_table_separator = TrailingTableSeparator::Keep;
        } else if (value == "never") {
            trailing_table_separator = TrailingTableSeparator::Never;
        } else if (value == "always") {
            trailing_table_separator = TrailingTableSeparator::Always;
        } else if (value == "smart") {
            trailing_table_separator = TrailingTableSeparator::Smart;
        }
    }

    IF_EXIST(call_arg_parentheses) {
        if (value == "keep") {
            call_arg_parentheses = CallArgParentheses::Keep;
        } else if (value == "remove") {
            call_arg_parentheses = CallArgParentheses::Remove;
        } else if (value == "remove_string_only") {
            call_arg_parentheses = CallArgParentheses::RemoveStringOnly;
        } else if (value == "remove_table_only") {
            call_arg_parentheses = CallArgParentheses::RemoveTableOnly;
        } else if (value == "always") {
            call_arg_parentheses = CallArgParentheses::Always;
        }
    }

    IF_EXIST(continuation_indent) {
        if (IsNumber(value)) {
            continuation_indent.SetAll(std::stoull(value));
        }
    }

    IF_EXIST(continuation_indent.before_block) {
        if (IsNumber(value)) {
            continuation_indent.before_block = std::stoull(value);
        }
    }

    IF_EXIST(continuation_indent.in_expr) {
        if (IsNumber(value)) {
            continuation_indent.in_expr = std::stoull(value);
        }
    }

    IF_EXIST(continuation_indent.in_table) {
        if (IsNumber(value)) {
            continuation_indent.in_table = std::stoull(value);
        }
    }

    BOOL_OPTION(detect_end_of_line)

    BOOL_OPTION(insert_final_newline)

    BOOL_OPTION(space_around_table_field_list)

    BOOL_OPTION(space_before_attribute)

    BOOL_OPTION(space_before_function_open_parenthesis)

    BOOL_OPTION(space_before_function_call_open_parenthesis)

    BOOL_OPTION(space_before_closure_open_parenthesis)

    IF_EXIST(space_before_function_call_single_arg) {
        if (value == "true" || value == "always") {
            space_before_function_call_single_arg.SetAll(FunctionSingleArgSpace::Always);
        } else if (value == "only_string") {
            space_before_function_call_single_arg.string = FunctionSingleArgSpace::Always;
            space_before_function_call_single_arg.table = FunctionSingleArgSpace::None;
        } else if (value == "only_table") {
            space_before_function_call_single_arg.string = FunctionSingleArgSpace::None;
            space_before_function_call_single_arg.table = FunctionSingleArgSpace::Always;
        } else if (value == "false" || value == "none") {
            space_before_function_call_single_arg.SetAll(FunctionSingleArgSpace::None);
        }
    }

    IF_EXIST(space_before_function_call_single_arg.string) {
        if (value == "true" || value == "always") {
            space_before_function_call_single_arg.string = FunctionSingleArgSpace::Always;
        } else if (value == "false" || value == "none") {
            space_before_function_call_single_arg.string = FunctionSingleArgSpace::None;
        } else if (value == "keep") {
            space_before_function_call_single_arg.string = FunctionSingleArgSpace::Keep;
        }
    }

    IF_EXIST(space_before_function_call_single_arg.table) {
        if (value == "true" || value == "always") {
            space_before_function_call_single_arg.table = FunctionSingleArgSpace::Always;
        } else if (value == "false" || value == "none") {
            space_before_function_call_single_arg.table = FunctionSingleArgSpace::None;
        } else if (value == "keep") {
            space_before_function_call_single_arg.table = FunctionSingleArgSpace::Keep;
        }
    }

    BOOL_OPTION(space_before_open_square_bracket)

    BOOL_OPTION(space_inside_function_call_parentheses)

    BOOL_OPTION(space_inside_function_param_list_parentheses)

    BOOL_OPTION(space_inside_square_brackets)

    BOOL_OPTION(space_around_table_append_operator)

    BOOL_OPTION(ignore_spaces_inside_function_call)

    IF_EXIST(space_before_inline_comment) {
        if (value == "keep") {
            space_before_inline_comment = SpaceBeforeInlineComment(0, SpaceBeforeInlineCommentStyle::Keep);
        } else if (IsNumber(value)) {
            space_before_inline_comment = SpaceBeforeInlineComment(std::stoi(value), SpaceBeforeInlineCommentStyle::Fixed);
        }
    }

    BOOL_OPTION(space_after_comment_dash)

    IF_EXIST(space_around_math_operator) {
        if (value == "true" || value == "always") {
            space_around_math_operator.SetAll(true);
        } else if (value == "false" || value == "none") {
            space_around_math_operator.SetAll(false);
        }
    }

    IF_EXIST(space_around_math_operator.exponent) {
        if (value == "true" || value == "always") {
            space_around_math_operator.exponent = true;
        } else if (value == "false" || value == "none") {
            space_around_math_operator.exponent = false;
        }
    }

    BOOL_OPTION(space_after_comma)

    BOOL_OPTION(space_after_comma_in_for_statement)

    IF_EXIST(space_around_concat_operator) {
        if (value == "true" || value == "always") {
            space_around_concat_operator = SpaceAroundStyle::Always;
        } else if (value == "false" || value == "none") {
            space_around_concat_operator = SpaceAroundStyle::None;
        } else if (value == "no_space_asym") {
            space_around_concat_operator = SpaceAroundStyle::NoSpaceAsym;
        }
    }

    BOOL_OPTION(space_around_logical_operator)

    IF_EXIST(space_around_assign_operator) {
        if (value == "true" || value == "always") {
            space_around_assign_operator = SpaceAroundStyle::Always;
        } else if (value == "false" || value == "none") {
            space_around_assign_operator = SpaceAroundStyle::None;
        } else if (value == "no_space_asym") {
            space_around_assign_operator = SpaceAroundStyle::NoSpaceAsym;
        }
    }

    BOOL_OPTION(align_call_args)

    BOOL_OPTION(align_function_params)

    IF_EXIST(align_continuous_assign_statement) {
        if (value == "true" || value == "when_extra_space") {
            align_continuous_assign_statement = ContinuousAlign::WhenExtraSpace;
        } else if (value == "always") {
            align_continuous_assign_statement = ContinuousAlign::Always;
        } else if (value == "false" || value == "none") {
            align_continuous_assign_statement = ContinuousAlign::None;
        }
    }

    IF_EXIST(align_continuous_rect_table_field) {
        if (value == "true" || value == "when_extra_space") {
            align_continuous_rect_table_field = ContinuousAlign::WhenExtraSpace;
        } else if (value == "always") {
            align_continuous_rect_table_field = ContinuousAlign::Always;
        } else if (value == "false" || value == "none") {
            align_continuous_rect_table_field = ContinuousAlign::None;
        }
    }

    NUMBER_OPTION(align_continuous_line_space)

    BOOL_OPTION(align_if_branch)

    IF_EXIST(align_array_table) {
        if (value == "true" || value == "normal") {
            align_array_table = AlignArrayTable::Normal;
        } else if (value == "contain_curly") {
            align_array_table = AlignArrayTable::ContainCurly;
        } else if (value == "false" || value == "none") {
            align_array_table = AlignArrayTable::None;
        }
    }

    IF_EXIST(align_chain_expr) {
        if (value == "none") {
            align_chain_expr = AlignChainExpr::None;
        } else if (value == "always") {
            align_chain_expr = AlignChainExpr::Always;
        } else if (value == "only_call_stmt") {
            align_chain_expr = AlignChainExpr::OnlyCallStmt;
        }
    }

    BOOL_OPTION(align_continuous_similar_call_args)

    BOOL_OPTION(align_continuous_inline_comment)

    BOOL_OPTION(never_indent_before_if_condition)

    if (align_if_branch) {
        never_indent_before_if_condition = true;
    }

    BOOL_OPTION(never_indent_comment_on_if_branch)

    BOOL_OPTION(keep_indents_on_empty_lines)

    BOOL_OPTION(allow_non_indented_comments)

    std::vector<std::pair<std::string, LineSpace &>> fieldList = {
            {"line_space_after_if_statement",              line_space_after_if_statement             },
            {"line_space_after_do_statement",              line_space_after_do_statement             },
            {"line_space_after_while_statement",           line_space_after_while_statement          },
            {"line_space_after_repeat_statement",          line_space_after_repeat_statement         },
            {"line_space_after_for_statement",             line_space_after_for_statement            },
            {"line_space_after_local_or_assign_statement", line_space_after_local_or_assign_statement},
            {"line_space_after_function_statement",        line_space_after_function_statement       },
            {"line_space_after_expression_statement",      line_space_after_expression_statement     },
            {"line_space_after_comment",                   line_space_after_comment                  },
            {"line_space_around_block",                    line_space_around_block                   }
    };

    for (auto &lineOption: fieldList) {
        if (configMap.count(lineOption.first)) {
            std::string value = configMap.at(lineOption.first);
            FunctionOption option;
            option.Parse(value);

            if (option.GetKey() == "keep") {
                lineOption.second = LineSpace(LineSpaceType::Keep);
            } else if (option.GetKey() == "min") {
                auto p1 = option.GetParam(0);
                if (!p1.empty() && IsNumber(p1)) {
                    lineOption.second = LineSpace(LineSpaceType::Min, std::stoi(p1));
                }
            } else if (option.GetKey() == "fixed") {
                auto p1 = option.GetParam(0);
                if (!p1.empty() && IsNumber(p1)) {
                    lineOption.second = LineSpace(LineSpaceType::Fixed, std::stoi(p1));
                }
            } else if (option.GetKey() == "max") {
                auto p1 = option.GetParam(0);
                if (!p1.empty() && IsNumber(p1)) {
                    lineOption.second = LineSpace(LineSpaceType::Max, std::stoi(p1));
                }
            }
        }
    }

    BOOL_OPTION(break_all_list_when_line_exceed)

    BOOL_OPTION(auto_collapse_lines)

    BOOL_OPTION(break_before_braces)

    IF_EXIST(break_table_list) {
        if (value == "never") {
            break_table_list = BreakTableList::Never;
        } else if (value == "smart") {
            break_table_list = BreakTableList::Smart;
        } else if (value == "lazy") {
            break_table_list = BreakTableList::Lazy;
        }
    }

    BOOL_OPTION(ignore_space_after_colon)

    BOOL_OPTION(remove_call_expression_list_finish_comma)

    IF_EXIST(end_statement_with_semicolon)  {
        if (value == "keep") {
            end_statement_with_semicolon = EndStmtWithSemicolon::Keep;
        } else if (value == "always") {
            end_statement_with_semicolon = EndStmtWithSemicolon::Always;
        } else if (value == "replace_with_newline") {
            end_statement_with_semicolon = EndStmtWithSemicolon::ReplaceWithNewline;
        } else if (value == "same_line") {
            end_statement_with_semicolon = EndStmtWithSemicolon::SameLine;
        } else if (value == "never") {
            end_statement_with_semicolon = EndStmtWithSemicolon::Never;
        }
    }
}
