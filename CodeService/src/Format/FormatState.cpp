#include "CodeService/Format/FormatState.h"
#include "CodeService/Format/Analyzer/SpaceAnalyzer.h"
#include "CodeService/Format/Analyzer/IndentationAnalyzer.h"
#include "CodeService/Format/Analyzer/LineBreakAnalyzer.h"
#include "CodeService/Format/Analyzer/AlignAnalyzer.h"
#include "CodeService/Format/Analyzer/TokenAnalyzer.h"
#include "CodeService/Format/Analyzer/PreferenceAnalyzer.h"

FormatState::FormatState()
        : _currentWidth(0) {
}

std::size_t &FormatState::GetCurrentWidth() {
    return _currentWidth;
}

EndOfLine FormatState::GetEndOfLine() const {
    return _formatStyle.detect_end_of_line ?
           _fileEndOfLine : _formatStyle.end_of_line;;
}

void FormatState::SetFormatStyle(LuaStyle &style) {
    _formatStyle = style;
}

const LuaStyle &FormatState::GetStyle() const {
    return _formatStyle;
}

void FormatState::SetDiagnosticStyle(LuaDiagnosticStyle &style) {
    _diagnosticStyle = style;
}

const LuaDiagnosticStyle &FormatState::GetDiagnosticStyle() const {
    return _diagnosticStyle;
}

bool FormatState::IsNewLine() const {
    return _currentWidth == 0;
}


void FormatState::Analyze(const LuaSyntaxTree &t) {
    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();
    AddAnalyzer<LineBreakAnalyzer>();
    AddAnalyzer<AlignAnalyzer>();
    AddAnalyzer<TokenAnalyzer>();
    AddAnalyzer<PreferenceAnalyzer>();

    _fileEndOfLine = t.GetFile().GetEndOfLine();
    for (const auto &analyzer: _analyzers) {
        if (analyzer) {
            analyzer->Analyze(*this, t);
        }
    }
}

std::array<std::unique_ptr<FormatAnalyzer>, static_cast<std::size_t>(FormatAnalyzerType::Count)> &
FormatState::GetAnalyzers() {
    return _analyzers;
}

void FormatState::AddRelativeIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent) {
    if (_indentStack.empty()) {
        _indentStack.emplace(syntaxNoe, 0, 0);
        return;
    }
    auto top = _indentStack.top();
    switch (_formatStyle.indent_style) {
        case IndentStyle::Space: {
            _indentStack.emplace(syntaxNoe, top.SpaceSize + indent, top.TabSize);
            break;
        }
        case IndentStyle::Tab: {
            auto tabIndent = indent / _formatStyle.tab_width;
            auto spaceIndent = indent % _formatStyle.tab_width;
            _indentStack.emplace(syntaxNoe, top.SpaceSize + spaceIndent, top.TabSize + tabIndent);
            break;
        }
    }
}

void FormatState::AddInvertIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent) {
    if (_indentStack.empty()) {
        _indentStack.emplace(syntaxNoe, 0, 0);
        return;
    }
    auto top = _indentStack.top();
    switch (_formatStyle.indent_style) {
        case IndentStyle::Space: {
            std::size_t spaceSize = 0;
            if (top.SpaceSize > indent) {
                spaceSize = top.SpaceSize - indent;
            }
            _indentStack.emplace(syntaxNoe, spaceSize, top.TabSize);
            break;
        }
        case IndentStyle::Tab: {
            auto tabIndent = indent / _formatStyle.tab_width;
            auto spaceIndent = indent % _formatStyle.tab_width;
            std::size_t spaceSize = 0;
            std::size_t tabSize = 0;
            if (top.SpaceSize > spaceIndent) {
                spaceSize = top.SpaceSize - spaceIndent;
            }
            if (top.TabSize > tabIndent) {
                tabSize = top.TabSize - tabIndent;
            }

            _indentStack.emplace(syntaxNoe, spaceSize, tabSize);
            break;
        }
    }
}

void FormatState::RecoverIndent() {
    if (_indentStack.empty()) {
        return;
    }
    _indentStack.pop();
}

IndentState FormatState::GetCurrentIndent() const {
    if (_indentStack.empty()) {
        return IndentState(LuaSyntaxNode(0), 0, 0);
    }
    return _indentStack.top();
}

