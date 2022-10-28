#pragma once

#include <vector>
#include "LuaParser/Ast/LuaAstNode.h"
#include "LuaParser/Ast/LuaAstTree.h"

class FormatBuilder {
public:
    FormatBuilder();

    void FormatAnalyze(const LuaAstTree &t);

    void SpaceAround(LuaAstNode& n, std::size_t space = 1);
    void SpaceLeft(LuaAstNode& n, std::size_t space = 1);
    void SpaceRight(LuaAstNode& n, std::size_t space = 1);
private:

};