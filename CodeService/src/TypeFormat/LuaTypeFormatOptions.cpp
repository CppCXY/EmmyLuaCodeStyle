#include "CodeService/TypeFormat/LuaTypeFormatOptions.h"

LuaTypeFormatOptions LuaTypeFormatOptions::ParseFrom(std::map<std::string, std::string, std::less<>>& stringOptions)
{
	LuaTypeFormatOptions options;

	if(stringOptions.count("auto_complete_end"))
	{
		options.auto_complete_end = stringOptions["auto_complete_end"] == "true";
	}

	if(stringOptions.count("format_line"))
	{
		options.format_line = stringOptions["format_line"] == "true";
	}

	return options;
}
