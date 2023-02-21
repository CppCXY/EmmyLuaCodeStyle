#include "CodeService/Format/FormatState.h"
#include "CodeService/Format/Analyzer/SpaceAnalyzer.h"
#include "CodeService/Format/Analyzer/IndentationAnalyzer.h"
#include "CodeService/Format/Analyzer/LineBreakAnalyzer.h"
#include "CodeService/Format/Analyzer/AlignAnalyzer.h"
#include "CodeService/Format/Analyzer/TokenAnalyzer.h"

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

    _fileEndOfLine = t.GetFile().GetEndOfLine();
    for (const auto &analyzer: _analyzers) {
        if (analyzer) {
            analyzer->Analyze(*this, t);
        }
    }

    for (const auto &analyzer: _analyzers) {
        if (analyzer) {
            analyzer->ComplexAnalyze(*this, t);
        }
    }
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

void FormatState::DfsForeach(std::vector<LuaSyntaxNode> &startNodes,
                             const LuaSyntaxTree &t,
                             const FormatState::FormatHandle &enterHandle) {
    std::vector<Traverse> traverseStack;
    for (auto it = startNodes.rbegin(); it != startNodes.rend(); it++) {
        traverseStack.emplace_back(*it, TraverseEvent::Enter);
    }

    // 非递归深度优先遍历
    FormatResolve resolve;
    while (!traverseStack.empty()) {
        Traverse traverse = traverseStack.back();
        resolve.Reset();
        if (traverse.Event == TraverseEvent::Enter) {
            traverseStack.back().Event = TraverseEvent::Exit;
            for (auto &analyzer: _analyzers) {
                analyzer->Query(*this, traverse.Node, t, resolve);
            }
            auto children = traverse.Node.GetChildren(t);
            // 不采用 <range>
            for (auto rIt = children.rbegin(); rIt != children.rend(); rIt++) {
                traverseStack.emplace_back(*rIt, TraverseEvent::Enter);
            }

            if (resolve.GetIndentStrategy() != IndentStrategy::None) {
                auto indent = resolve.GetIndent();
                if (indent == 0) {
                    if (_formatStyle.indent_style == IndentStyle::Space) {
                        indent = _formatStyle.indent_size;
                    } else {
                        indent = _formatStyle.tab_width;
                    }
                }

                switch (resolve.GetIndentStrategy()) {
                    case IndentStrategy::Relative: {
                        AddRelativeIndent(traverse.Node, indent);
                        break;
                    }
                    case IndentStrategy::InvertRelative: {
                        AddInvertIndent(traverse.Node, indent);
                        break;
                    }
                    case IndentStrategy::Absolute: {
                        AddAbsoluteIndent(traverse.Node, indent);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }

            enterHandle(traverse.Node, t, resolve);
        } else {
            traverseStack.pop_back();
            if (GetCurrentIndent().SyntaxNode.GetIndex() == traverse.Node.GetIndex()) {
                RecoverIndent();
            }
        }
    }
}

void FormatState::AddAbsoluteIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent) {
    switch (_formatStyle.indent_style) {
        case IndentStyle::Space: {
            _indentStack.emplace(syntaxNoe, indent, 0);
            break;
        }
        case IndentStyle::Tab: {
            auto tabIndent = indent / _formatStyle.tab_width;
            auto spaceIndent = indent % _formatStyle.tab_width;
            _indentStack.emplace(syntaxNoe, spaceIndent, tabIndent);
            break;
        }
    }
}

