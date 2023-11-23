#pragma once

#include "FormatContext.h"

class LuaCheck {
public:
    bool Check(const FormatContext &context);

private:
    bool CheckSingleFile(const FormatContext &context, std::string_view path, std::string &&sourceText, LuaStyle style);

    bool CheckWorkspace(const FormatContext &context);
};
