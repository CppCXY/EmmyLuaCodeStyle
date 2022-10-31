#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "Analyzer/FormatAnalyzer.h"
#include "Types.h"

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

    void Indenter(LuaSyntaxNode& n);
private:

    std::unordered_map<std::size_t, IndentState> _indentStates;
    std::unordered_map<std::size_t, std::size_t> _leftSpaces;
    std::unordered_map<std::size_t, std::size_t> _rightSpaces;

    std::vector<std::shared_ptr<FormatAnalyzer>> _analyzers;
};