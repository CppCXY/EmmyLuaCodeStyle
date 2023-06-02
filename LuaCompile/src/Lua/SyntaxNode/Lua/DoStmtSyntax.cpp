#include "DoStmtSyntax.h"

DoStmtSyntax::DoStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

BodySyntax DoStmtSyntax::GetBody(const LuaSyntaxTree &t) const {
    return GetMember<BodySyntax>(t);
}
