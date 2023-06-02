#pragma

#include "DocBaseSyntax.h"

class DocDeprecatedSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::DocDeprecated;
    }

    explicit DocDeprecatedSyntax(LuaNodeOrToken n = LuaNodeOrToken());

};
