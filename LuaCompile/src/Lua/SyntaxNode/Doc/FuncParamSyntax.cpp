#include "FuncParamSyntax.h"

FuncParamSyntax::FuncParamSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view FuncParamSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}
