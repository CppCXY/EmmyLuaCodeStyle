#include "CodeService/LuaFormatOptions.h"
#include <fstream>
#include <filesystem>
#include <regex>
#include "CodeService/FormatElement/MinLineElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"

static std::vector<std::string> Split(std::string_view source, std::string_view separator)
{
	if (source.empty() || separator.empty())
	{
		return {};
	}

	std::size_t sepLen = separator.size();
	std::vector<std::string> result;

	while (true)
	{
		const std::size_t sepIndex = source.find_first_of(separator, 0);
		if (sepIndex == std::string_view::npos)
		{
			result.push_back(std::string(source));
			break;
		}
		else
		{
			result.push_back(std::string(source.substr(0, sepIndex)));
			source = source.substr(sepIndex + sepLen);
		}
	}
	return result;
}

LuaFormatOptions LuaFormatOptions::ParseFromEditorConfig(std::string_view filename)
{
	std::filesystem::path filePath(filename);
	std::string realPath = filePath.string();
	std::fstream fin(realPath, std::ios::in | std::ios::binary);

	if (!fin.is_open())
	{
		return {};
	}
	std::stringstream s;
	s << fin.rdbuf();
	fin.close();
	std::string text = s.str();

	auto lines = Split(text, "\n");

	bool luaSectionFounded = false;
	std::regex comment = std::regex(R"(^\s*(;|#))");
	std::regex luaSection = std::regex(R"(^\s*\[\s*([\w\.\*]+)\s*\]\s*$)");
	std::regex valueRegex = std::regex(R"(^\s*([\w\d_]+)\s*=\s*([\w\:\._]+)\s*$)");
	std::map<std::string, std::string> optionMap;

	for (auto& line : lines)
	{
		if (std::regex_search(line, comment))
		{
			continue;
		}

		std::smatch m;

		if (std::regex_search(line, m, luaSection))
		{
			if (m.str(1) == "*.lua")
			{
				luaSectionFounded = true;
			}
			else
			{
				luaSectionFounded = false;
			}

			continue;
		}

		if (luaSectionFounded)
		{
			if (std::regex_search(line, m, valueRegex))
			{
				optionMap.insert({m.str(1), m.str(2)});
			}
		}
	}


	return ParseOptionsFromMap(optionMap);
}

LuaFormatOptions LuaFormatOptions::ParseOptionsFromMap(std::map<std::string, std::string>& optionMap)
{
	LuaFormatOptions options;
	if (optionMap.count("use_tab"))
	{
		options.use_tab = optionMap["use_tab"] == "true";
	}

	if (optionMap.count("indent"))
	{
		options.indent = std::stoi(optionMap["indent"]);
	}

	if (optionMap.count("continuation_indent"))
	{
		options.continuation_indent = std::stoi(optionMap["indent"]);
	}

	if (optionMap.count("align_call_args"))
	{
		options.align_call_args = optionMap["align_call_args"] == "true";
	}

	if (optionMap.count("keep_one_space_between_call_args_and_bracket"))
	{
		options.keep_one_space_between_call_args_and_bracket =
			optionMap["keep_one_space_between_call_args_and_bracket"] == "true";
	}

	if (optionMap.count("keep_one_space_between_table_and_bracket"))
	{
		options.keep_one_space_between_table_and_bracket =
			optionMap["keep_one_space_between_table_and_bracket"] == "true";
	}

	if (optionMap.count("align_table_field_to_first_field"))
	{
		options.align_table_field_to_first_field = optionMap["align_table_field_to_first_field"] == "true";
	}

	if (optionMap.count("continuous_assign_statement_align_to_equal_sign"))
	{
		options.continuous_assign_statement_align_to_equal_sign =
			optionMap["continuous_assign_statement_align_to_equal_sign"] == "true";
	}

	if (optionMap.count("continuous_assign_table_field_align_to_equal_sign"))
	{
		options.continuous_assign_table_field_align_to_equal_sign =
			optionMap["continuous_assign_table_field_align_to_equal_sign"] == "true";
	}

	if (optionMap.count("line_separator"))
	{
		auto lineSeparatorSymbol = optionMap["line_separator"];
		if (lineSeparatorSymbol == "CRLF")
		{
			options.line_separator = "\r\n";
		}
		else if (lineSeparatorSymbol == "LF")
		{
			options.line_separator = "\n";
		}
	}

	if (optionMap.count("max_line_length"))
	{
		options.max_line_length = std::stoi(optionMap["max_line_length"]);
	}

	std::vector<std::pair<std::string, std::shared_ptr<FormatElement>&>> fieldList = {
		{"keep_line_after_if_statement", options.keep_line_after_if_statement},
		{"keep_line_after_do_statement", options.keep_line_after_do_statement},
		{"keep_line_after_while_statement", options.keep_line_after_while_statement},
		{"keep_line_after_repeat_statement", options.keep_line_after_repeat_statement},
		{"keep_line_after_for_statement", options.keep_line_after_for_statement},
		{"keep_line_after_local_or_assign_statement", options.keep_line_after_local_or_assign_statement},
		{"keep_line_after_function_define_statement", options.keep_line_after_function_define_statement}
	};
	std::regex minLineRegex = std::regex(R"(minLine:\s*(\d+))");
	std::regex keepLineRegex = std::regex(R"(keepLine:\s*(\d+))");
	for (auto& keepLineOption : fieldList)
	{
		auto it = optionMap.find(keepLineOption.first);
		if (it != optionMap.end())
		{
			if (it->second == "keepLine")
			{
				keepLineOption.second = std::make_shared<KeepLineElement>();
				continue;
			}
			std::smatch m;

			if (std::regex_search(it->second, m, minLineRegex))
			{
				keepLineOption.second = std::make_shared<MinLineElement>(std::stoi(m.str(1)));
				continue;
			}

			if (std::regex_search(it->second, m, keepLineRegex))
			{
				keepLineOption.second = std::make_shared<KeepLineElement>(std::stoi(m.str(1)));
			}
		}
	}
	return options;
}

LuaFormatOptions::LuaFormatOptions()
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
