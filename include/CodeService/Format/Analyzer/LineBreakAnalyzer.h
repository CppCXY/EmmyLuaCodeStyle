#pragma once

#include <unordered_set>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"
#include "FormatStrategy.h"

class LineBreakAnalyzer: public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(LineBreakAnalyzer)

    LineBreakAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve& resolve) override;

    void BreakAfter(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t line = 1);

    void BreakAfter(LuaSyntaxNode n, const LuaSyntaxTree &t, LineSpace lineSpace);

    void BreakBefore(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t line = 1);

    void MarkLazyBreak(LuaSyntaxNode n, const LuaSyntaxTree &t, LineBreakStrategy strategy);
private:

    void AnalyzeExprList(FormatState &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    void AnalyzeExpr(FormatState &f, LuaSyntaxNode& expr, const LuaSyntaxTree &t);

    void AnalyzeConditionExpr(FormatState &f, LuaSyntaxNode& expr, const LuaSyntaxTree &t);

    void AnalyzeNameList(FormatState &f, LuaSyntaxNode& nameList, const LuaSyntaxTree &t);

    void AnalyzeSuffixedExpr(FormatState &f, LuaSyntaxNode& expr, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, LineBreakData> _lineBreaks;
};