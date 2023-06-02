
#include "EnumFieldSyntax.h"
EnumFieldSyntax::EnumFieldSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view EnumFieldSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
