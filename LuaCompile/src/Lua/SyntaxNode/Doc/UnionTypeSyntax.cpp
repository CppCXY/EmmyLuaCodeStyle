#include "UnionTypeSyntax.h"

UnionTypeSyntax::UnionTypeSyntax(LuaNodeOrToken n) : TypeSyntax(n) {
}

std::vector<TypeSyntax> UnionTypeSyntax::GetUnionTypes(const LuaSyntaxTree &t) const {
    return GetMembers<TypeSyntax>(t);
}
