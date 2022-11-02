#pragma once

#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class LineBreakAnalyzer: public FormatAnalyzer {
public:
    LineBreakAnalyzer();

    void Analyze(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) override;

    void Process(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) override;
private:
    void AnalyzeExpr(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeTableFieldList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeCallList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);
};