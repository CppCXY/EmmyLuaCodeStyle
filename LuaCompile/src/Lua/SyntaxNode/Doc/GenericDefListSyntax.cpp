#include "GenericDefListSyntax.h"

GenericDefListSyntax::GenericDefListSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}
std::vector<std::string_view> GenericDefListSyntax::GetDefNames(const LuaSyntaxTree &t) const {
    std::vector<std::string_view> result;
    for (auto token: _node.GetChildTokens(LuaTokenKind::TK_NAME, t)) {
        result.push_back(token.GetText(t));
    }
    return result;
}
