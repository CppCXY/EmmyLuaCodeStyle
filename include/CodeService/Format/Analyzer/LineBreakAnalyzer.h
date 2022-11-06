#pragma once

#include <unordered_set>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"
#include "FormatStrategy.h"

class LineBreakAnalyzer: public FormatAnalyzer {
public:
    LineBreakAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Process(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) override;

    void BreakTo(LuaSyntaxNode n, LineBreakStrategy strategy);
private:
    void AnalyzeExpr(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeTableFieldList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeCallList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    std::unordered_set<std::size_t> _mark;
};