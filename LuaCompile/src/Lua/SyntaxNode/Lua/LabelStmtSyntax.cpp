#include "LabelStmtSyntax.h"

LabelStmtSyntax::LabelStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

std::string_view LabelStmtSyntax::GetLabelName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
