#pragma once

#include "DocBaseSyntax.h"
#include "TypeSyntaxList.h"

class DocTypeSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocType;
    }

    explicit DocTypeSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    TypeSyntaxList GetTypeList(const LuaSyntaxTree &t) const;
};
