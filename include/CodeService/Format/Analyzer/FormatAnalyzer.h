#pragma once

#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "LuaParser/Ast/LuaSyntaxNode.h"

class FormatBuilder;

class FormatAnalyzer {
public:
    FormatAnalyzer();

    virtual void Analyze(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) = 0;

    virtual void Process(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) = 0;
};