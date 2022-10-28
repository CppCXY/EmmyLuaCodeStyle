#pragma once

#include "LuaParser/Ast/LuaAstTree.h"
#include "LuaParser/Ast/LuaAstNode.h"
#include "CodeService/Format/FormatBuilder.h"

class FormatAnalyzer {
public:
    FormatAnalyzer();

    virtual void Analyze(FormatBuilder &f, std::vector<LuaAstNode> &nodes, const LuaAstTree &t) = 0;
};