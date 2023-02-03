#include "CodeService/TypeFormat/LuaTypeFormatOptions.h"

LuaTypeFormatOptions
LuaTypeFormatOptions::ParseFromMap(std::map<std::string, std::string, std::less<>> &stringOptions) {
    LuaTypeFormatOptions options;

    if (stringOptions.count("auto_complete_end")) {
        options.auto_complete_end = stringOptions["auto_complete_end"] == "true";
    }

    if (stringOptions.count("format_line")) {
        options.format_line = stringOptions["format_line"] == "true";
    }

    if (stringOptions.count("auto_complete_table_sep")) {
        options.auto_complete_table_sep = stringOptions["auto_complete_table_sep"] == "true";
    }

    return options;
}
