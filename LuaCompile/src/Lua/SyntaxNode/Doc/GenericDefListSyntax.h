#pragma once

#include "DocBaseSyntax.h"

class GenericDefListSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::GenericDefList;
    }

    explicit GenericDefListSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<std::string_view> GetDefNames(const LuaSyntaxTree &t) const;
};
