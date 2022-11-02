#pragma once

#include "CodeService/Config/LuaStyleEnum.h"
#include "LuaParser/Types/TextRange.h"

struct IndentState {
    IndentState(IndentStyle style = IndentStyle::Space)
            : Style(style),
              Size(0) {}

    IndentStyle Style;
    std::size_t Size;
};

struct FormatChange {
    FormatChange()
            :  Start(0), Length(0) {}

    std::size_t Start;
    std::size_t Length;
    std::string NewString;
};