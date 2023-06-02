#include "DocInterfaceSyntax.h"
DocInterfaceSyntax::DocInterfaceSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view DocInterfaceSyntax::GetName(const LuaSyntaxTree &t) {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

GenericDefListSyntax DocInterfaceSyntax::GetGenericDefList(const LuaSyntaxTree &t) const {
    return GetMember<GenericDefListSyntax>(t);
}

TypeSyntaxList DocInterfaceSyntax::GetExtendTypeList(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntaxList>(t);
}
