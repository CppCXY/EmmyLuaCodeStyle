#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include "LuaStyleEnum.h"

class LuaDiagnosticStyle {
public:
    LuaDiagnosticStyle() = default;

    bool code_style_check = true;

    bool name_style_check = true;

    bool spell_check = true;
};
