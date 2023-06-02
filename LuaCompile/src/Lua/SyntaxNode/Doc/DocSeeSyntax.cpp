#include "DocSeeSyntax.h"

DocSeeSyntax::DocSeeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view DocSeeSyntax::GetContent(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
