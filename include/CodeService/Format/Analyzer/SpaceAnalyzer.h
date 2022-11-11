#pragma once

#include <optional>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class SpaceAnalyzer : public FormatAnalyzer {
public:
    SpaceAnalyzer();

    void Analyze(FormatBuilder &f, const LuaSyntaxTree &t) override;

    void Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve& resolve) override;

private:
    void SpaceAround(LuaSyntaxNode &n, std::size_t space = 1);

    void SpaceLeft(LuaSyntaxNode &n, std::size_t space = 1);

    void SpaceRight(LuaSyntaxNode &n, std::size_t space = 1);

    std::optional<std::size_t> GetLeftSpace(LuaSyntaxNode& n) const;

    std::optional<std::size_t> GetRightSpace(LuaSyntaxNode& n) const;

    void ProcessSpace(FormatBuilder &f, const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right);

    std::unordered_map<std::size_t, std::size_t> _leftSpaces;
    std::unordered_map<std::size_t, std::size_t> _rightSpaces;
};
