#pragma once

#include <vector>
#include <array>
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

    std::string GetRangeFormatResult(FormatRange &range, const LuaSyntaxTree &t);

    template<class T>
    void AddAnalyzer() {
        _analyzers[static_cast<std::size_t>(T::Type)] = std::make_unique<T>();
    }

    template<class T>
    T *GetAnalyzer() {
        auto &ptr = _analyzers[static_cast<std::size_t>(T::Type)];
        if (ptr) {
            return dynamic_cast<T *>(ptr.get());
        }
        return nullptr;
    }

    const LuaStyle &GetStyle() const;

    bool ShouldMeetIndent() const;

    std::size_t GetCurrentWidth() const;

private:
    void AddRelativeIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent);

    void AddInvertIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent);

    void RecoverIndent();

    void DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    void DoRangeResolve(FormatRange &range, LuaSyntaxNode &syntaxNode,
                        const LuaSyntaxTree &t, FormatResolve &resolve);

    void ExitResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    void WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void WriteSpace(std::size_t space);

    void WriteLine(std::size_t line);

    void WriteIndent();

    void WriteChar(char ch);

    void WriteText(std::string_view text);

    LuaStyle _style;
    EndOfLine _fileEndOfLine;
    std::stack<IndentState> _indentStack;
    std::size_t _writeLineWidth;
    std::array<std::unique_ptr<FormatAnalyzer>, static_cast<std::size_t>(FormatAnalyzerType::Count)> _analyzers;
    std::string _formattedText;
};