#include "ArrayTypeSyntax.h"

ArrayTypeSyntax::ArrayTypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

TypeSyntax ArrayTypeSyntax::GetBaseType(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntax>(t);
}
