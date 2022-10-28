#pragma once

#include "FormatAnalyzer.h"

class SpaceAnalyzer: public FormatAnalyzer {
public:
    SpaceAnalyzer();

    void Analyze(FormatBuilder &f, std::vector<LuaAstNode> &nodes, const LuaAstTree &t) override;

private:



};