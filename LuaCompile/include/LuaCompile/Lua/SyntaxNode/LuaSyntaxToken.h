#pragma once

#include "LuaCompile/Lua/SyntaxTree/LuaNodeOrToken.h"

class LuaSyntaxToken {
    explicit LuaSyntaxToken(LuaNodeOrToken n);

    LuaNodeOrToken GetToken() const;

    LuaTokenKind GetKind(const LuaSyntaxTree &t) const;

protected:
    LuaNodeOrToken _token;
};

