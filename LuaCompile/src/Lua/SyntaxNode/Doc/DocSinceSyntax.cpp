#include "DocSinceSyntax.h"

DocSinceSyntax::DocSinceSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view DocSinceSyntax::GetContent(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
