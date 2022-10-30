#pragma once

#include <vector>
#include <memory>
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "Analyzer/FormatAnalyzer.h"

class FormatBuilder {
public:
    FormatBuilder();

    void FormatAnalyze(const LuaSyntaxTree &t);

    template<class T>
    void AddAnalyzer() {
        _analyzers.push_back(std::make_shared<T>());
    }

    void SpaceAround(LuaSyntaxNode &n, std::size_t space = 1);

    void SpaceLeft(LuaSyntaxNode &n, std::size_t space = 1);

    void SpaceRight(LuaSyntaxNode &n, std::size_t space = 1);

private:

    std::vector<std::shared_ptr<FormatAnalyzer>> _analyzers;
};