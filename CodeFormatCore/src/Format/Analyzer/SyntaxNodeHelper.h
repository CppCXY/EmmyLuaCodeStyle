#pragma once

#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include <vector>

namespace helper {
std::vector<LuaSyntaxNode>
CollectBinaryOperator(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t,
                      const std::function<bool(LuaTokenKind)> &predicated);
};
