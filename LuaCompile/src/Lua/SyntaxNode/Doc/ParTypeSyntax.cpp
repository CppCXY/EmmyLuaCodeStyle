#include "ParTypeSyntax.h"

ParTypeSyntax::ParTypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

TypeSyntax ParTypeSyntax::GetInnerType(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntax>(t);
}
