#pragma once

#include "LuaSyntaxNodeKind.h"

enum class LuaSyntaxMultiKind {
    Expression,
    Statement,
};

// try detail::multi_match
namespace detail {
namespace multi_match {
bool Match(LuaSyntaxMultiKind k, LuaSyntaxNodeKind syntaxNodeKind);
bool ExpressionMatch(LuaSyntaxNodeKind k);
bool StatementMatch(LuaSyntaxNodeKind k);
}
}