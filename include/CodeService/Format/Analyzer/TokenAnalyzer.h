#pragma once

#include <unordered_map>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class TokenAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(TokenAnalyzer)

    TokenAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void Mark(LuaSyntaxNode &n, const LuaSyntaxTree &t, TokenStrategy strategy);
private:
    void TableFieldAddSep(FormatBuilder &f, LuaSyntaxNode &n, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, TokenStrategy> _tokenStrategies;
};