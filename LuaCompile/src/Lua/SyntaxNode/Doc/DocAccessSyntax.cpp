#include "DocAccessSyntax.h"


DocAccessSyntax::DocAccessSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

AccessVisibility DocAccessSyntax::GetVisibility(const LuaSyntaxTree &t) const {
    switch (_node.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::DocPublic: {
            return AccessVisibility::Public;
        }
        case LuaSyntaxNodeKind::DocProtected: {
            return AccessVisibility::Protected;
        }
        case LuaSyntaxNodeKind::DocPrivate: {
            return AccessVisibility::Private;
        }
        default: {
            break;
        }
    }
    return AccessVisibility::Public;
}
