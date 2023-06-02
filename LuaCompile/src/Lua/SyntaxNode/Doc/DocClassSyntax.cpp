#include "DocClassSyntax.h"

DocClassSyntax::DocClassSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view DocClassSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

GenericDefListSyntax DocClassSyntax::GetGenericDefList(const LuaSyntaxTree &t) const {
    return GetMember<GenericDefListSyntax>(t);
}

TypeSyntaxList DocClassSyntax::GetExtendTypeList(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntaxList>(t);
}
