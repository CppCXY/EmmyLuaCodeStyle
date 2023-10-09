#pragma once

#include "FormatAnalyzerType.h"
#include "FormatResolve.h"
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"

class FormatState;

class FormatAnalyzer {
public:
    FormatAnalyzer();

    virtual ~FormatAnalyzer();

    virtual FormatAnalyzerType GetType() const = 0;

    virtual void Analyze(FormatState &f, const LuaSyntaxTree &t) = 0;

    virtual void ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t){};

    virtual void Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) = 0;

    virtual void OnFormatMessage(FormatState &f, FormatEvent event, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t){};
};
