#pragma once

#include "FormatAnalyzer.h"

class PreferenceAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(PreferenceAnalyzer)

    PreferenceAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

private:
    void AnalyzeAutoSplitOrJoinLines(FormatState &f, const LuaSyntaxTree &t);
};
