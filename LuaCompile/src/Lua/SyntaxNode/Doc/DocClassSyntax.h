#pragma once

#include "DocBaseSyntax.h"
#include "GenericDefListSyntax.h"
#include "TypeSyntaxList.h"

class DocClassSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocClass;
    }

    explicit DocClassSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree &t) const;

    GenericDefListSyntax GetGenericDefList(const LuaSyntaxTree &t) const;

    TypeSyntaxList GetExtendTypeList(const LuaSyntaxTree &t) const;
};
