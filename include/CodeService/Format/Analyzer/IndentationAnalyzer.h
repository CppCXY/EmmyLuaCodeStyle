#pragma once

#include <stack>
#include <optional>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"
#include "CodeService/Config/LuaStyleEnum.h"
#include "CodeService/Format/Types.h"

struct IndentSize {
    std::size_t SpaceSize = 0;
    std::size_t TabSize = 0;
};

class IndentationAnalyzer : public FormatAnalyzer {
public:
    IndentationAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve& resolve) override;
private:
    void Indenter(LuaSyntaxNode &n);

    void Indenter(LuaSyntaxNode &n, IndentStyle style, std::size_t size);

    std::optional<IndentState> GetIndentState(LuaSyntaxNode& n) const;

    void AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode& exprList, const LuaSyntaxTree &t);

    IndentStyle _style;
    std::unordered_map<std::size_t, IndentState> _indentStates;
    std::stack<IndentSize> _indentSize;
};



