#include "TableExprSyntax.h"

TableExprSyntax::TableExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::vector<TableFieldSyntax> TableExprSyntax::GetFields(const LuaSyntaxTree &t) const {
    return GetMembers<TableFieldSyntax>(t);
}
