#pragma once

#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class IndentationAnalyzer : public FormatAnalyzer {
public:
    IndentationAnalyzer();

    void Analyze(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) override;

    void Process(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) override;
private:
    void AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);
};



