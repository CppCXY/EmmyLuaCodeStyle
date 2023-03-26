#pragma once

#include "FormatAnalyzer.h"

class FormatDocAnalyze : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(FormatDocAnalyze)

    enum class FormatType {
        DisableNext,
        Disable,
        Set
    };

    FormatDocAnalyze();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

private:
    void AnalyzeDocFormat(LuaSyntaxNode n, FormatState &f, const LuaSyntaxTree &t);

//    std::unordered_map<std::size_t, >
};