#pragma once

#include "LuaLinterType.h"


class LuaLinterOptions
{
public:
	void ParseFromJson(nlohmann::json json);

	LinterMaxLineLength max_line_length;
	LinterIndentStyle indent_style;
	LinterQuoteStyle quote_style;
	LinterWhiteSpace white_space;
	LinterAlignStyle align_style;
	LinterLineSpace line_space;
	LinterNameStyle name_style;
};

