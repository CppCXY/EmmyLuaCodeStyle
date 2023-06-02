#pragma once

#include "LuaCompile/Lua/Kind/LuaSyntaxNodeKind.h"

enum class LuaSyntaxMultiKind {
    Expression,
    Statement,
    Type,
    Tag
};

// try detail::multi_match
namespace detail {
namespace multi_match {
bool Match(LuaSyntaxMultiKind k, LuaSyntaxNodeKind syntaxNodeKind);
bool ExpressionMatch(LuaSyntaxNodeKind k);
bool StatementMatch(LuaSyntaxNodeKind k);
bool TypeMatch(LuaSyntaxNodeKind k);
bool DocTagMatch(LuaSyntaxNodeKind k);
}// namespace multi_match
}// namespace detail