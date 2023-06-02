#pragma once

#include "DocBaseSyntax.h"
#include "GenericDeclareSyntax.h"

class DocGenericSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocGeneric;
    }

    explicit DocGenericSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<GenericDeclareSyntax> GetDeclares(const LuaSyntaxTree& t) const;
};

