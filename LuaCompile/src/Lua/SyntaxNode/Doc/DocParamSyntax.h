#pragma once

#include "DocBaseSyntax.h"
#include "TypeSyntax.h"

class DocParamSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocParam;
    }

    explicit DocParamSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree& t) const;

    bool IsNullable(const LuaSyntaxTree& t) const;

    TypeSyntax GetType(const LuaSyntaxTree& t) const;
};
