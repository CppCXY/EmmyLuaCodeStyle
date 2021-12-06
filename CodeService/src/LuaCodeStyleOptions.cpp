#include "CodeService/LuaCodeStyleOptions.h"
#include <filesystem>
#include <regex>
#include "CodeService/FormatElement/MinLineElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"

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
