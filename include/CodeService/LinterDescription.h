#pragma once


enum class LinterDescription
{
	normal,
	no_linter,

	keep_line_after_if_statement,
	keep_line_after_do_statement,
	keep_line_after_while_statement,
	keep_line_after_repeat_statement,
	keep_line_after_for_statement,
	keep_line_after_local_or_assign_statement,
	keep_line_after_function_define_statement,

	wrapper_space
};
