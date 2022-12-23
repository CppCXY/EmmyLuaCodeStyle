#pragma once

#include <optional>
#include <unordered_set>
#include <unordered_map>
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

    void SpaceIgnore(LuaSyntaxNode &n, const LuaSyntaxTree &t);

private:
    std::optional<std::size_t> GetLeftSpace(LuaSyntaxNode &n) const;

    std::optional<std::size_t> GetRightSpace(LuaSyntaxNode &n) const;

    std::size_t ProcessSpace(FormatBuilder &f, const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right);

//    void PushDiagnostic(FormatBuilder &f,
//                        LuaSyntaxNode &node, LuaSyntaxNode &next, const LuaSyntaxTree &t,
//                        size_t space, size_t shouldSpace,
//                        DiagnosticResolve& resolve
//                        );
//
//    std::string GetAdditionalNote(LuaSyntaxNode &left, LuaSyntaxNode& right, const LuaSyntaxTree& t);

    std::unordered_map<std::size_t, std::size_t> _leftSpaces;
    std::unordered_map<std::size_t, std::size_t> _rightSpaces;
    std::unordered_set<std::size_t> _ignoreSpace;
};
