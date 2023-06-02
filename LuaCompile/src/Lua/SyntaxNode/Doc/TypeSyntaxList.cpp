#include "TypeSyntaxList.h"

TypeSyntaxList::TypeSyntaxList(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::vector<TypeSyntax> TypeSyntaxList::GetTypeList(const LuaSyntaxTree &t) const {
    return GetMembers<TypeSyntax>(t);
}
