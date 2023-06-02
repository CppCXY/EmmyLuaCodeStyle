
#include "DocOverloadSyntax.h"
DocOverloadSyntax::DocOverloadSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

FuncTypeSyntax DocOverloadSyntax::GetOverloadFunc(const LuaSyntaxTree &t) const {
    return GetMember<FuncTypeSyntax>(t);
}
