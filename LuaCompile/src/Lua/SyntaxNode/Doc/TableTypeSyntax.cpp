#include "TableTypeSyntax.h"

TableTypeSyntax::TableTypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::vector<TableTypeFieldSyntax> TableTypeSyntax::GetFields(const LuaSyntaxTree &t) const {
    return GetMembers<TableTypeFieldSyntax>(t);
}
