#pragma once

#include <optional>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class SpaceAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(SpaceAnalyzer)

    SpaceAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void SpaceAround(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space = 1, bool force = true);

    void SpaceLeft(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space = 1, bool force = true);

    void SpaceRight(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space = 1, bool force = true);

private:
    std::optional<std::size_t> GetLeftSpace(LuaSyntaxNode &n) const;

    std::optional<std::size_t> GetRightSpace(LuaSyntaxNode &n) const;

    std::size_t ProcessSpace(FormatBuilder &f, const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right);

    std::unordered_map<std::size_t, std::size_t> _leftSpaces;
    std::unordered_map<std::size_t, std::size_t> _rightSpaces;
};
