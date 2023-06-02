#pragma once

#include "DocBaseSyntax.h"
#include "EnumFieldSyntax.h"
#include "TypeSyntax.h"

class DocEnumSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocEnum;
    }

    explicit DocEnumSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree& t) const;

    TypeSyntax GetBaseType(const LuaSyntaxTree& t) const;

    std::vector<EnumFieldSyntax> GetFields(const LuaSyntaxTree& t) const;
};
