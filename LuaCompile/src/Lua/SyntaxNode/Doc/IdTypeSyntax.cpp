#include "IdTypeSyntax.h"

IdTypeSyntax::IdTypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view IdTypeSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
