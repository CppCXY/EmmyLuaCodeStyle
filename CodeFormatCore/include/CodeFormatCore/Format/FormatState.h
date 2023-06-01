#pragma once

#include "CodeFormatCore/Config/LuaDiagnosticStyle.h"
#include "CodeFormatCore/Config/LuaStyle.h"
#include "Analyzer/FormatAnalyzer.h"
#include "Types.h"
#include <array>

class FormatState {
public:
    enum class Mode {
        Format,
        Diagnostic
    };

    using FormatHandle = std::function<void(LuaSyntaxNode &syntaxNode,
                                            const LuaSyntaxTree &t,
                                            FormatResolve &resolve)>;

    explicit FormatState(Mode mode = Mode::Format);

    void SetFormatStyle(LuaStyle &style);

    const LuaStyle &GetStyle() const;

    void SetDiagnosticStyle(LuaDiagnosticStyle &style);

    const LuaDiagnosticStyle &GetDiagnosticStyle() const;

    EndOfLine GetEndOfLine() const;

    bool IsNewLine(LuaSyntaxNode &n, const LuaSyntaxTree &t) const;

    std::size_t &CurrentWidth();

    void AddAbsoluteIndent(LuaSyntaxNode syntaxNoe, std::size_t indent);

    void AddRelativeIndent(LuaSyntaxNode syntaxNoe, std::size_t indent);

    void AddInvertIndent(LuaSyntaxNode syntaxNoe, std::size_t indent);

    void AddSameIndent(LuaSyntaxNode syntaxNode);

    void RecoverIndent();

    IndentState GetCurrentIndent() const;

    void AddIgnore(IndexRange range);

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

    void Analyze(const LuaSyntaxTree &t);

    // 深度优先处理格式
    void DfsForeach(std::vector<LuaSyntaxNode> &startNodes,
                    const LuaSyntaxTree &t,
                    const FormatHandle &enterHandle);

    void StopDfsForeach();

    Mode GetMode() const;

    void Notify(FormatEvent event, LuaSyntaxNode n, const LuaSyntaxTree &t);

private:
    LuaStyle _formatStyle;
    LuaDiagnosticStyle _diagnosticStyle;
    EndOfLine _fileEndOfLine;
    std::size_t _currentWidth;
    std::stack<IndentState> _indentStack;
    Mode _mode;
    IndexRange _ignoreRange;
    bool _foreachContinue;
    std::array<std::unique_ptr<FormatAnalyzer>, static_cast<std::size_t>(FormatAnalyzerType::Count)> _analyzers;
};