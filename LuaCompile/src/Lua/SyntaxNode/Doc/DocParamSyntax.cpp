#include "DocParamSyntax.h"
DocParamSyntax::DocParamSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::string_view DocParamSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

bool DocParamSyntax::IsNullable(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NULLABLE, t).IsToken(t);
}

TypeSyntax DocParamSyntax::GetType(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntax>(t);
}
