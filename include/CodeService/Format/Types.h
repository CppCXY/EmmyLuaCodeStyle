#pragma once

#include "CodeService/Config/LuaStyleEnum.h"

struct IndentState {
    IndentState()
            : Style(IndentStyle::Space),
              Size(0) {}

    IndentStyle Style;
    std::size_t Size;
};