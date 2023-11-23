#pragma once

#include "FormatContext.h"
#include <filesystem>
#include <string>


class LuaFormat {
public:
    bool Reformat(const FormatContext &context);

    bool RangeReformat(const FormatContext &context);

private:
    bool ReformatSingleFile(const FormatContext &context, std::string_view outPath, std::string &&sourceText, LuaStyle style);

    bool ReformatWorkspace(const FormatContext &context);
};
