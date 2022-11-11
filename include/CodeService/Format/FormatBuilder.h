#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "Analyzer/FormatAnalyzer.h"
#include "Analyzer/SpaceAnalyzer.h"
#include "Analyzer/IndentationAnalyzer.h"
#include "Analyzer/LineBreakAnalyzer.h"
#include "Analyzer/FormatResolve.h"
#include "Types.h"

class FormatBuilder {
public:
    FormatBuilder();

    void FormatAnalyze(const LuaSyntaxTree &t);

    std::string GetFormatResult(const LuaSyntaxTree &t);

    template<class T>
    void AddAnalyzer() {
        _analyzers.push_back(std::make_shared<T>());
    }

private:
    void DoResolve(LuaSyntaxNode& syntaxNode, const LuaSyntaxTree &t, FormatResolve& resolve);

    void WriteSyntaxNode(LuaSyntaxNode& syntaxNode, const LuaSyntaxTree &t);

    void WriteSpace(std::size_t space);

    void WriteLine(std::size_t line);

    std::vector<std::shared_ptr<FormatAnalyzer>> _analyzers;
    std::string _formattedText;
};