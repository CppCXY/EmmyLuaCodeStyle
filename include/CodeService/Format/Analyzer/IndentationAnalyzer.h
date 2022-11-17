#pragma once

#include <stack>
#include <optional>
#include <unordered_set>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"
#include "CodeService/Config/LuaStyleEnum.h"
#include "CodeService/Format/Types.h"

class IndentationAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(IndentationAnalyzer)

    IndentationAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve& resolve) override;

    void Indenter(LuaSyntaxNode &n, const LuaSyntaxTree &t, IndentData indentData=IndentData());
private:
    std::optional<IndentState> GetIndentState(LuaSyntaxNode& n) const;

    void AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, IndentData> _indent;
};



