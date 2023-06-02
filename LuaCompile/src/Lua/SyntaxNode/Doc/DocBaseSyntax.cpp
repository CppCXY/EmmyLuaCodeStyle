#include "DocBaseSyntax.h"

DocBaseSyntax::DocBaseSyntax(LuaNodeOrToken n) : LuaSyntaxNode(n) {
}

bool DocBaseSyntax::CanCast(LuaSyntaxNodeKind kind) {
    return static_cast<int>(kind) <= static_cast<int>(LuaSyntaxNodeKind::NormalComment) && static_cast<int>(kind) >= static_cast<int>(LuaSyntaxNodeKind::Comment);
}
