#pragma once

#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "FormatResolve.h"

class FormatBuilder;

class FormatAnalyzer {
public:
    FormatAnalyzer();

    virtual ~FormatAnalyzer();

    virtual void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) = 0;

    virtual void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve& resolve) = 0;
};
