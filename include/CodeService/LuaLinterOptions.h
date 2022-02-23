#pragma once

#include "LuaLinterType.h"


class LuaLinterOptions
{
public:
	void ParseFromJson(nlohmann::json json);

	int max_line_length;
	LinterIndentStyle indent_style;
	LinterQuoteStyle quote_style;
	LinterWhiteSpace white_space;
	LinterLineSpace line_space;
	LinterNameStyle name_style;
};

