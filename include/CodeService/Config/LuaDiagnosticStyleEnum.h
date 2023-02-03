#pragma once

#include <string>

enum class NameStyleType {
    Off,
    CamelCase,
    PascalCase,
    SnakeCase,
    UpperSnakeCase,
    Same,
    Pattern,
};

struct NameStyleRule {
    explicit NameStyleRule(NameStyleType type)
            : Type(type) {}

    NameStyleRule(NameStyleType type, std::string_view param)
            : Type(type), Param(param) {}

    NameStyleType Type;
    std::string Param;
};