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

private:
    FormatResolve Resolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t);

    SpaceAnalyzer _spaceAnalyzer;
    IndentationAnalyzer _indentationAnalyzer;
    LineBreakAnalyzer _lineBreakAnalyzer;

    std::string _formattedText;
};