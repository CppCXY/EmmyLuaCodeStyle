#pragma once

#include <unordered_set>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"
#include "FormatStrategy.h"

class LineBreakAnalyzer: public FormatAnalyzer {
public:
    LineBreakAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve& resolve) override;

    void BreakTo(LuaSyntaxNode n, LineBreakStrategy strategy);

    void BreakAfter(LuaSyntaxNode n, std::size_t line = 1);
private:
    void BreakAnalyze(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t length);

    void AnalyzeExpr(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeTableFieldList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeCallList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeConditionExpr(FormatBuilder &f, LuaSyntaxNode& expr, const LuaSyntaxTree &t);

    void AnalyzeNameList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    std::unordered_set<std::size_t> _mark;
};