#pragma once

#include <functional>
#include <string_view>
#include <memory>
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Config/LuaDiagnosticStyleEnum.h"

class NameStyleRuleMatcher {
public:
    bool Match(LuaSyntaxNode &n, const LuaSyntaxTree &t, const std::vector<NameStyleRule> &rules);

private:
    static bool SnakeCase(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    static bool UpperSnakeCase(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    static bool CamelCase(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    static bool PascalCase(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    static bool Same(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::string_view param);

    static bool PatternMatch(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::string_view pattern);
};
