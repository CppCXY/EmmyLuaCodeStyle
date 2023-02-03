#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include "LuaDiagnosticStyleEnum.h"

class LuaDiagnosticStyle {
public:
    LuaDiagnosticStyle() = default;

    bool code_style_check = true;

    bool name_style_check = true;

    bool spell_check = true;

    std::vector<NameStyleRule> local_name_style = {NameStyleRule(NameStyleType::SnakeCase)};

    std::vector<NameStyleRule> function_param_name_style = {NameStyleRule(NameStyleType::SnakeCase)};

    std::vector<NameStyleRule> function_name_style = {NameStyleRule(NameStyleType::SnakeCase)};

    std::vector<NameStyleRule> local_function_name_style = {NameStyleRule(NameStyleType::SnakeCase)};

    std::vector<NameStyleRule> table_field_name_style = {NameStyleRule(NameStyleType::SnakeCase)};

    std::vector<NameStyleRule> global_variable_name_style = {
            NameStyleRule(NameStyleType::SnakeCase),
            NameStyleRule(NameStyleType::UpperSnakeCase)
    };

    std::vector<NameStyleRule> module_name_style = {
            NameStyleRule(NameStyleType::Same, "m"),
            NameStyleRule(NameStyleType::SnakeCase)
    };

    std::vector<NameStyleRule> require_module_name_style = {
            NameStyleRule(NameStyleType::SnakeCase),
            NameStyleRule(NameStyleType::PascalCase),
    };

    std::vector<NameStyleRule> class_name_style = {
            NameStyleRule(NameStyleType::SnakeCase),
            NameStyleRule(NameStyleType::PascalCase)
    };
};
