#include "GotoStmtSyntax.h"

GotoStmtSyntax::GotoStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::string_view GotoStmtSyntax::GetLabel(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
