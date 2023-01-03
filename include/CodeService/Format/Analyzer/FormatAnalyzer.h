#pragma once

#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "FormatAnalyzerType.h"
#include "FormatResolve.h"
#include "CodeService/Diagnostic/StyleDiagnostic.h"

class FormatBuilder;

class FormatAnalyzer {
public:
    FormatAnalyzer();

    virtual ~FormatAnalyzer();

    virtual FormatAnalyzerType GetType() const = 0;

    virtual void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) = 0;

    virtual void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) = 0;

    virtual void
    ExitQuery(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {};

    virtual void Diagnostic(StyleDiagnostic &d, const LuaSyntaxTree &t) {};
};
