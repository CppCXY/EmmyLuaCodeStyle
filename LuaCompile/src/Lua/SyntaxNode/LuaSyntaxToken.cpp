#include "LuaCompile/Lua/SyntaxNode/LuaSyntaxToken.h"

LuaSyntaxToken::LuaSyntaxToken(LuaNodeOrToken n)
    : _token(n) {
}

LuaNodeOrToken LuaSyntaxToken::GetToken() const {
    return _token;
}

LuaTokenKind LuaSyntaxToken::GetKind(const LuaSyntaxTree &t) const {
    return _token.GetTokenKind(t);
}
