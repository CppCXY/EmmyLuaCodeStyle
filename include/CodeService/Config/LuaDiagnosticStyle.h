#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include "LuaStyleEnum.h"

class LuaDiagnosticStyle {
public:
    LuaDiagnosticStyle() = default;

    // [basic]
    /*
     * 缩进风格
     */
    IndentStyle indent_style = IndentStyle::Space;

    /*
     * 缩进的空白数
     */
    std::size_t indent_size = 4;

    /*
     * tab的width
     */
    std::size_t tab_width = 4;

    /*
     * 引号风格
     */
    QuoteStyle quote_style = QuoteStyle::None;

    /*
     * 行宽检查
     */
    std::size_t max_line_length = 120;

    // [space]

    bool enable_space_check = true;

    bool space_around_math_operator = true;


};