#pragma once

#include "CodeFormatCore/Format/Types.h"
#include "FormatAnalyzer.h"
#include <unordered_map>

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

    void Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void AddIgnoreRange(const IndexRange& range, const LuaSyntaxTree &t);

    std::vector<IndexRange> GetIgnores() const;
private:

    void AnalyzeDocFormat(LuaSyntaxNode n, FormatState &f, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, IndexRange> _ignores;
};