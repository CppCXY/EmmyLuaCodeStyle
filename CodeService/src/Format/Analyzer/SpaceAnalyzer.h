#pragma once

#include "CodeService/Format/Analyzer/FormatAnalyzer.h"

class SpaceAnalyzer : public FormatAnalyzer {
public:
    SpaceAnalyzer();

    void Analyze(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) override;

    void Process(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) override;

private:
    void ProcessSpace(FormatBuilder &f, const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right);
};