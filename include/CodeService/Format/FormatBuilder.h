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
#include "CodeService/Config/LuaStyle.h"
#include "Types.h"

class FormatBuilder {
public:
    explicit FormatBuilder(LuaStyle &style);

    void FormatAnalyze(const LuaSyntaxTree &t);

    std::string GetFormatResult(const LuaSyntaxTree &t);

    template<class T>
    void AddAnalyzer() {
        _analyzers.push_back(std::make_shared<T>());
    }

    template<class T>
    std::shared_ptr<T> GetAnalyzer() {
        for(auto a: _analyzers) {
            if(a->GetType() == T::Type){
                return std::dynamic_pointer_cast<T>(a);
            }
        }
        return nullptr;
    }

    const LuaStyle &GetStyle() const;

    std::size_t CurrentLineLength() const;
private:
    void AddIndent(LuaSyntaxNode &syntaxNoe);

    void RecoverIndent();

    void DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    void ExitResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    void WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void WriteSpace(std::size_t space);

    void WriteLine(std::size_t line);

    void WriteIndent();

    LuaStyle _style;
    std::stack<IndentState> _indentStack;
    std::vector<std::shared_ptr<FormatAnalyzer>> _analyzers;
    std::string _formattedText;
};