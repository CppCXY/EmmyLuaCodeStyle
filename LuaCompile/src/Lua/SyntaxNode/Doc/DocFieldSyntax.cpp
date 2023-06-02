#include "DocFieldSyntax.h"

DocFieldSyntax::DocFieldSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

AccessVisibility DocFieldSyntax::GetVisibility(const LuaSyntaxTree &t) const {
    auto access = _node.GetChildToken(LuaTokenKind::TK_ACCESS, t).GetText(t);
    if (access == "public") {
        return AccessVisibility::Public;
    } else if (access == "protected") {
        return AccessVisibility::Protected;
    } else if (access == "private") {
        return AccessVisibility::Private;
    }
    return AccessVisibility::Public;
}

bool DocFieldSyntax::IsNullable(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NULLABLE, t).IsToken(t);
}

std::string_view DocFieldSyntax::GetName(const LuaSyntaxTree &t) const {
    return _node.GetChildToken(LuaTokenKind::TK_NAME, t).GetText(t);
}

FieldIndexSyntax DocFieldSyntax::GetIndex(const LuaSyntaxTree &t) const {
    return GetMember<FieldIndexSyntax>(t);
}

TypeSyntax DocFieldSyntax::GetType(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntax>(t);
}
