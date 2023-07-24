#pragma once

#include <map>
#include <string>

class LuaTypeFormatFeatures {
public:
    static LuaTypeFormatFeatures From(std::map<std::string, std::string, std::less<>> &features);

    bool format_line = true;
    bool auto_complete_end = true;
    bool auto_complete_table_sep = true;
    bool fix_indent = true;
};
