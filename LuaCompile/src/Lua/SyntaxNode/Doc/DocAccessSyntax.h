#pragma once

#include "DocBaseSyntax.h"
#include "Lua/Define/AccessVisibility.h"

class DocAccessSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        switch (kind) {
            case LuaSyntaxNodeKind::DocPublic:
            case LuaSyntaxNodeKind::DocPrivate:
            case LuaSyntaxNodeKind::DocProtected: {
                return true;
            }
            default: {
                return false;
            }
        }
    }

    explicit DocAccessSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    AccessVisibility GetVisibility(const LuaSyntaxTree &t) const;
};
