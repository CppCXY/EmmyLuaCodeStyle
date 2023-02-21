#pragma once

#include <unordered_map>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"


class AlignAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(AlignAnalyzer)

    AlignAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

private:
    void PushAlignGroup(AlignStrategy strategy, std::vector<std::size_t> &data);

    void PushNormalAlignGroup(std::size_t alignPos, std::vector<std::size_t> &data);

    void AnalyzeContinuousLocalOrAssign(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeContinuousRectField(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeContinuousArrayTableField(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeArrayTableAlign(FormatState &f, std::vector<LuaSyntaxNode> &arrayTable, const LuaSyntaxTree &t);

    void AnalyzeExpressionList(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeParamList(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeIfStatement(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void ResolveAlignGroup(FormatState &f, std::size_t groupIndex, AlignGroup &group, const LuaSyntaxTree &t);

    void AnalyzeContinuousSimilarCallArgs(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void AnalyzeInlineComment(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    std::vector<AlignGroup> _alignGroup;
    std::unordered_map<std::size_t, std::size_t> _startNodeToGroupIndex;
    std::unordered_map<std::size_t, std::size_t> _resolveGroupIndex;

    std::vector<std::vector<std::size_t>> _inlineCommentGroup;
};