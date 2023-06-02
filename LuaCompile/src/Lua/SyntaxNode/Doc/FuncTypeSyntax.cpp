#include "FuncTypeSyntax.h"

FuncTypeSyntax::FuncTypeSyntax(LuaNodeOrToken n) : TypeSyntax(n) {
}

FuncParamSyntaxList FuncTypeSyntax::GetFuncParamTypeList(const LuaSyntaxTree &t) const {
    return GetMember<FuncParamSyntaxList>(t);
}

TypeSyntax FuncTypeSyntax::GetReturnType(const LuaSyntaxTree &t) const {
    return GetMember<TypeSyntax>(t);
}
