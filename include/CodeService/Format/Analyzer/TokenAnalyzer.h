#pragma once

#include <unordered_map>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class TokenAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(TokenAnalyzer)

    TokenAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void Mark(LuaSyntaxNode &n, const LuaSyntaxTree &t, TokenStrategy strategy);

    bool IsRemove(LuaSyntaxNode &n, const LuaSyntaxTree &t) const;
private:
    void TableFieldAddSep(FormatState &f, LuaSyntaxNode &n, const LuaSyntaxTree &t);

    void AnalyzeTableField(FormatState &f, LuaSyntaxNode& syntaxNode , const LuaSyntaxTree &t);

    void AnalyzeCallExpression(FormatState &f, LuaSyntaxNode& syntaxNode , const LuaSyntaxTree &t);


    std::unordered_map<std::size_t, TokenStrategy> _tokenStrategies;
};