#include "CodeFormatCore/TypeFormat/LuaTypeFormatFeatures.h"

LuaTypeFormatFeatures
LuaTypeFormatFeatures::From(std::map<std::string, std::string, std::less<>> &features) {
    LuaTypeFormatFeatures formatFeatures;

    if (features.count("auto_complete_end")) {
        formatFeatures.auto_complete_end = features["auto_complete_end"] == "true";
    }

    if (features.count("format_line")) {
        formatFeatures.format_line = features["format_line"] == "true";
    }

    if (features.count("auto_complete_table_sep")) {
        formatFeatures.auto_complete_table_sep = features["auto_complete_table_sep"] == "true";
    }

    return formatFeatures;
}
