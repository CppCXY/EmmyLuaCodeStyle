#pragma once

#include "FormatAnalyzer.h"

class IntentionAnalyzer: public FormatAnalyzer {
public:
    IntentionAnalyzer();

    void Analyze(FormatBuilder &f, std::vector<LuaAstNode> &nodes, const LuaAstTree &t) override;
};



