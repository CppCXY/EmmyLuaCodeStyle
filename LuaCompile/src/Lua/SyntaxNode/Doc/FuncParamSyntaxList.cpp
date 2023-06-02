#include "FuncParamSyntaxList.h"

FuncParamSyntaxList::FuncParamSyntaxList(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::vector<FuncParamSyntax> FuncParamSyntaxList::GetFuncParams(const LuaSyntaxTree &t) const {
    return GetMembers<FuncParamSyntax>(t);
}
