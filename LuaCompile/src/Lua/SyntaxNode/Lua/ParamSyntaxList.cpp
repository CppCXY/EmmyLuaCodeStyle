#include "ParamSyntaxList.h"

ParamSyntaxList::ParamSyntaxList(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::vector<std::string_view> ParamSyntaxList::GetParams(const LuaSyntaxTree &t) const {
    std::vector<std::string_view> results;
    for (auto p: _node.GetChildTokens(LuaTokenKind::TK_NAME, t)) {
        results.emplace_back(p.GetText(t));
    }
    return results;
}
