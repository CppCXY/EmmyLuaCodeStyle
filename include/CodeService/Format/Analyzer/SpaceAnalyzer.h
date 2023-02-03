#pragma once

#include <optional>
#include <unordered_set>
#include <unordered_map>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class SpaceAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(SpaceAnalyzer)

    SpaceAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void SpaceAround(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space = 1);

    void SpaceLeft(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space = 1);

    void SpaceRight(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space = 1);

    void SpaceIgnore(LuaSyntaxNode &n, const LuaSyntaxTree &t);

private:
//    std::optional<std::size_t> GetLeftSpace(LuaSyntaxNode &n) const;

    std::optional<std::size_t> GetRightSpace(LuaSyntaxNode &n) const;

    std::size_t ProcessSpace(const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right);

    std::unordered_map<std::size_t, std::size_t> _rightSpaces;
    std::unordered_set<std::size_t> _ignoreSpace;


};
