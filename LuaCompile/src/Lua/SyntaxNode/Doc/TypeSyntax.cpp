#include "TypeSyntax.h"

TypeSyntax::TypeSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

bool TypeSyntax::CanCast(LuaSyntaxNodeKind kind) {
    return detail::multi_match::TypeMatch(kind);
}
