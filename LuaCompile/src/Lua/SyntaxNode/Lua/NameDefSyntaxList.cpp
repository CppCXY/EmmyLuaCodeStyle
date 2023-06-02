#include "NameDefSyntaxList.h"

NameDefSyntaxList::NameDefSyntaxList(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::vector<NameDefSyntax> NameDefSyntaxList::GetList(const LuaSyntaxTree &t) const {
    return GetMembers<NameDefSyntax>(t);
}
