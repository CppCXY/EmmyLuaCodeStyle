#include "DocAliasSyntax.h"
DocAliasSyntax::DocAliasSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view DocAliasSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

TypeSyntax DocAliasSyntax::GetType(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntax>(t);
}
