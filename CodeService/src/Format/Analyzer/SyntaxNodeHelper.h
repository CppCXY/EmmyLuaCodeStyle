#pragma once

#include <vector>
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"

namespace helper {
    std::vector<LuaSyntaxNode>
    CollectBinaryOperator(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t,
                          const std::function<bool(LuaTokenKind)> &predicated);
};
