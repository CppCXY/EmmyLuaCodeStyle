#pragma once

#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "LuaParser/Ast/LuaSyntaxNode.h"

class FormatBuilder;

class FormatAnalyzer {
public:
    FormatAnalyzer();

    ~FormatAnalyzer();

    virtual void Analyze(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) = 0;

    virtual void Process(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) = 0;
};
