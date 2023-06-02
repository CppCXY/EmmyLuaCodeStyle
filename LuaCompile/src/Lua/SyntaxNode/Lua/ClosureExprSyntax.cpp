#include "ClosureExprSyntax.h"

ClosureExprSyntax::ClosureExprSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

ParamSyntaxList ClosureExprSyntax::GetParamList(const LuaSyntaxTree &t) const {
    return GetMember<ParamSyntaxList>(t);
}

BodySyntax ClosureExprSyntax::GetBody(const LuaSyntaxTree &t) const {
    return GetMember<BodySyntax>(t);
}
