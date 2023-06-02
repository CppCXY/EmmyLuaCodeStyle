#include "GenericTypeSyntax.h"

GenericTypeSyntax::GenericTypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view GenericTypeSyntax::GetBaseName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

std::vector<TypeSyntax> GenericTypeSyntax::GetTypeList(const LuaSyntaxTree &t) const {
    return GetMembers<TypeSyntax>(t);
}
