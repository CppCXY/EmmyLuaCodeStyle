#pragma once

#include "FormatAnalyzer.h"
#include <unordered_map>
#include <unordered_set>

class SpaceAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(SpaceAnalyzer)

    enum class SpacePriority : std::size_t {
        Normal = 0,
        First,
    };

    SpaceAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void SpaceAround(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t space = 1, SpacePriority priority = SpacePriority::Normal);

    void SpaceLeft(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t space = 1, SpacePriority priority = SpacePriority::Normal);

    void SpaceRight(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t space = 1, SpacePriority priority = SpacePriority::Normal);

    void SpaceIgnore(LuaSyntaxNode n);

    void FunctionCallSingleArgSpace(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t);
private:
    // This is to ensure that the settings on the right take priority.
    struct SpaceData {
        SpaceData() : SpaceData(0) {}

        explicit SpaceData(std::size_t space, SpacePriority priority = SpacePriority::Normal) : Value(space), Priority(priority) {}

        std::size_t Value;
        SpacePriority Priority;
    };

    std::size_t GetRightSpace(LuaSyntaxNode n, bool& hasValue) const;

    std::size_t ProcessSpace(LuaSyntaxNode left, LuaSyntaxNode right, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, SpaceData> _rightSpaces;
    std::unordered_set<std::size_t> _ignoreSpace;
};
