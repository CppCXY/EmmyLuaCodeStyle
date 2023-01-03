#pragma once

#include "FormatAnalyzer.h"

class PreferenceAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(PreferenceAnalyzer)

    PreferenceAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

private:
    void AnalyzeAutoSplitOrJoinLines(FormatBuilder &f, const LuaSyntaxTree &t);
};
