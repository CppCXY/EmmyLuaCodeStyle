#include "NameDefSyntax.h"

NameDefSyntax::NameDefSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::string_view NameDefSyntax::GetName(const LuaSyntaxTree &t) const {
    return std::string_view();
}
