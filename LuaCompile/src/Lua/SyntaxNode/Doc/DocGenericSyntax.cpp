#include "DocGenericSyntax.h"

DocGenericSyntax::DocGenericSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::vector<GenericDeclareSyntax> DocGenericSyntax::GetDeclares(const LuaSyntaxTree &t) const {
    return GetMembers<GenericDeclareSyntax>(t);
}
