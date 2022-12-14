#pragma once

#include <unordered_map>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class AlignAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(AlignAnalyzer)

    AlignAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

private:
    void PushAlignGroup(AlignStrategy strategy, std::vector<std::size_t> &data);

    void AnalyzeContinuousLocalOrAssign(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeContinuousTableField(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeExpressionList(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeParamList(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeIfStatement(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void ResolveAlignGroup(FormatBuilder &f, std::size_t groupIndex, AlignGroup &group, const LuaSyntaxTree &t);

    std::vector<AlignGroup> _alignGroup;
    std::unordered_map<std::size_t, std::size_t> _startNodeToGroupIndex;
    std::unordered_map<std::size_t, std::size_t> _resolveGroupIndex;
};