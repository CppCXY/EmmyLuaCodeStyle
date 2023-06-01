#pragma once

#include "CodeFormatCore/Config/NameStyleRule.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include <functional>
#include <memory>
#include <string_view>

class NameStyleRuleMatcher {
public:
    bool Match(LuaSyntaxNode &n, const LuaSyntaxTree &t, const std::vector<NameStyleRule> &rules);

private:
    static bool SnakeCase(std::string_view text);

    static bool UpperSnakeCase(std::string_view text);

    static bool CamelCase(std::string_view text);

    static bool PascalCase(std::string_view text);

    static bool PatternMatch(std::string_view text, std::shared_ptr<PatternNameStyleData> data);
};
