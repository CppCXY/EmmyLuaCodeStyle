#pragma once

#include "DocBaseSyntax.h"
#include "GenericDefListSyntax.h"
#include "TypeSyntaxList.h"

class DocInterfaceSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocInterface;
    }
    explicit DocInterfaceSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::string_view GetName(const LuaSyntaxTree &t);

    GenericDefListSyntax GetGenericDefList(const LuaSyntaxTree &t) const;

    TypeSyntaxList GetExtendTypeList(const LuaSyntaxTree &t) const;
};
