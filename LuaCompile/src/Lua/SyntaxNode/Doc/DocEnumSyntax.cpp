#include "DocEnumSyntax.h"

DocEnumSyntax::DocEnumSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view DocEnumSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

TypeSyntax DocEnumSyntax::GetBaseType(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntax>(t);
}

std::vector<EnumFieldSyntax> DocEnumSyntax::GetFields(const LuaSyntaxTree &t) const {
    return GetMembers<EnumFieldSyntax>(t);
}
