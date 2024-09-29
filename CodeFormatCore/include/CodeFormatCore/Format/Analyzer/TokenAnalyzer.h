#pragma once

#include "FormatAnalyzer.h"
#include <unordered_map>

class TokenAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(TokenAnalyzer)

    TokenAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void MarkPrev(LuaSyntaxNode n, const LuaSyntaxTree &t, PrevTokenStrategy strategy);

    void Mark(LuaSyntaxNode n, const LuaSyntaxTree &t, TokenStrategy strategy);

    void MarkNext(LuaSyntaxNode n, const LuaSyntaxTree &t, NextTokenStrategy strategy);

    bool IsRemove(LuaSyntaxNode n, const LuaSyntaxTree &t) const;

private:
    void TableFieldAddSep(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t);

    void AnalyzeTableField(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t);

    void AnalyzeCallExpression(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t);

    void AnalyzeComment(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, TokenStrategy> _tokenStrategies;
    std::unordered_map<std::size_t, NextTokenStrategy> _nextTokenStrategies;
    std::unordered_map<std::size_t, PrevTokenStrategy> _prevTokenStrategies;
};