#include "StringTypeSyntax.h"
StringTypeSyntax::StringTypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view StringTypeSyntax::GetStringText(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
