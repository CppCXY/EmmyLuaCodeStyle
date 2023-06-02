#pragma once

#include "DocBaseSyntax.h"
#include "FuncTypeSyntax.h"

class DocOverloadSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocOverload;
    }

    explicit DocOverloadSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    FuncTypeSyntax GetOverloadFunc(const LuaSyntaxTree& t) const;
};
