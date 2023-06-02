#include "DocTypeSyntax.h"

DocTypeSyntax::DocTypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

TypeSyntaxList DocTypeSyntax::GetTypeList(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntaxList>(t);
}
