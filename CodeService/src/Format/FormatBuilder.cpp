#include "CodeService/Format/FormatBuilder.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/Analyzer/AlignAnalyzer.h"

FormatBuilder::FormatBuilder(LuaStyle &style)
        : _style(style),
          _writeLineWidth(0) {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {
    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();
    AddAnalyzer<LineBreakAnalyzer>();
    AddAnalyzer<AlignAnalyzer>();

    for (const auto &analyzer: _analyzers) {
        if (analyzer) {
            analyzer->Analyze(*this, t);
        }
    }
}

std::string FormatBuilder::GetFormatResult(const LuaSyntaxTree &t) {
    _formattedText.reserve(t.GetFile().GetSource().size());
    auto root = t.GetRootNode();
    std::vector<Traverse> traverseStack;
    traverseStack.emplace_back(root, TraverseEvent::Enter);
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

            DoResolve(traverse.Node, t, resolve);
        } else {
            traverseStack.pop_back();
            for (auto &analyzer: _analyzers) {
                analyzer->ExitQuery(*this, traverse.Node, t, resolve);
            }
            ExitResolve(traverse.Node, t, resolve);

            if (!_indentStack.empty()
                && _indentStack.top().SyntaxNode.GetIndex() == traverse.Node.GetIndex()) {
                RecoverIndent();
            }
        }
    }

    return _formattedText;
}

void FormatBuilder::WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto text = syntaxNode.GetText(t);
    _writeLineWidth += text.size();
    _formattedText.append(syntaxNode.GetText(t));
}

void FormatBuilder::AddIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent) {
    if (_indentStack.empty()) {
        _indentStack.emplace(syntaxNoe, 0, 0);
        return;
    }
    auto top = _indentStack.top();
    switch (_style.indent_style) {
        case IndentStyle::Space: {
            _indentStack.emplace(syntaxNoe, top.SpaceSize + indent, top.TabSize);
            break;
        }
        case IndentStyle::Tab: {
            auto tabIndent = indent / _style.tab_width;
            auto spaceIndent = indent % _style.tab_width;
            _indentStack.emplace(syntaxNoe, top.SpaceSize + spaceIndent, top.TabSize + tabIndent);
            break;
        }
    }
}

void FormatBuilder::RecoverIndent() {
    if (_indentStack.empty()) {
        return;
    }
    _indentStack.pop();
}

void FormatBuilder::DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (resolve.HasIndent()) {
        auto indent = resolve.GetIndent();
        if (indent == 0) {
            if (_style.indent_style == IndentStyle::Space) {
                indent = _style.indent_size;
            } else {
                indent = _style.tab_width;
            }
        }
        AddIndent(syntaxNode, indent);
    }

    if (syntaxNode.IsToken(t)) {
        if (!_formattedText.empty()) {
            auto lastChar = _formattedText.back();
            if (lastChar == '\n' || lastChar == '\r') {
                WriteIndent();
                auto indentAnalyzer = GetAnalyzer<IndentationAnalyzer>();
                if (indentAnalyzer) {
                    indentAnalyzer->MarkIndent(syntaxNode, t);
                }
            }
        }

        switch (resolve.GetPrevSpaceStrategy()) {
            case PrevSpaceStrategy::AlignPos: {
                auto pos = resolve.GetAlign();
                if (pos > _writeLineWidth) {
                    auto space = pos - _writeLineWidth;
                    WriteSpace(space);
                }
                break;
            }
            case PrevSpaceStrategy::AlignRelativeIndent: {
                auto relativePos = resolve.GetAlign();
                auto &indentState = _indentStack.top();
                auto pos = relativePos + indentState.SpaceSize + indentState.TabSize * _style.tab_width;
                if (pos > _writeLineWidth) {
                    auto space = pos - _writeLineWidth;
                    WriteSpace(space);
                }
                break;
            }
            default: {
                break;
            }
        }

        switch (resolve.GetTokenStrategy()) {
            case TokenStrategy::Origin: {
                WriteSyntaxNode(syntaxNode, t);
                break;
            }
            case TokenStrategy::Remove: {
                break;
            }
            case TokenStrategy::StringSingleQuote: {
                if (syntaxNode.GetTokenKind(t) == TK_STRING) {

                }
                break;
            }
            default: {
                break;
            }
        }

        switch (resolve.GetNextSpaceStrategy()) {
            case NextSpaceStrategy::Space: {
                WriteSpace(resolve.GetNextSpace());
                break;
            }
            case NextSpaceStrategy::LineBreak: {
                WriteLine(resolve.GetNextLine());
                break;
            }
            default: {
                break;
            }
        }
    }
}

void FormatBuilder::WriteSpace(std::size_t space) {
    if (space == 0) {
        return;
    } else if (space == 1) {
        _formattedText.push_back(' ');
    } else {
        auto size = _formattedText.size();
        _formattedText.resize(size + space, ' ');
    }
    _writeLineWidth += space;
}

void FormatBuilder::WriteLine(std::size_t line) {
    if (line == 0) {
        return;
    }
    auto endOfLine = _style.end_of_line;
    switch (endOfLine) {
        case EndOfLine::CRLF: {
            if (line == 1) {
                _formattedText.push_back('\r');
                _formattedText.push_back('\n');
            } else {
                auto index = _formattedText.size();
                _formattedText.resize(2 * line, '\n');
                for (; index < _formattedText.size(); index += 2) {
                    _formattedText[index] = '\r';
                }
            }
            break;
        }
        case EndOfLine::CR: {
            if (line == 1) {
                _formattedText.push_back('\r');
            } else {
                _formattedText.resize(_formattedText.size() + line, '\r');
            }
            break;
        }
        case EndOfLine::UNKNOWN:
        case EndOfLine::MIX:
        case EndOfLine::LF: {
            if (line == 1) {
                _formattedText.push_back('\n');
            } else {
                _formattedText.resize(_formattedText.size() + line, '\n');
            }
            break;
        }
    }
    _writeLineWidth = 0;
}

void FormatBuilder::WriteIndent() {
    if (_indentStack.empty()) {
        return;
    }
    auto &topLevelIndent = _indentStack.top();
    switch (_style.indent_style) {
        case IndentStyle::Space: {
            if (topLevelIndent.SpaceSize != 0) {
                auto oldSize = _formattedText.size();
                _formattedText.resize(oldSize + topLevelIndent.SpaceSize, ' ');
            }
            break;
        }
        case IndentStyle::Tab: {
            if (topLevelIndent.TabSize != 0) {
                auto oldSize = _formattedText.size();
                _formattedText.resize(oldSize + topLevelIndent.TabSize, '\t');
            }
            if (topLevelIndent.SpaceSize != 0) {
                auto oldSize = _formattedText.size();
                _formattedText.resize(oldSize + topLevelIndent.SpaceSize, ' ');
            }
            break;
        }
    }
    _writeLineWidth += topLevelIndent.SpaceSize + topLevelIndent.TabSize * _style.tab_width;
}

const LuaStyle &FormatBuilder::GetStyle() const {
    return _style;
}

void FormatBuilder::ExitResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {

}

bool FormatBuilder::ShouldMeetIndent() const {
    if (!_formattedText.empty()) {
        return _formattedText.back() == '\n' || _formattedText.back() == '\r';
    }
    return false;
}

std::size_t FormatBuilder::GetCurrentWidth() const {
    return _writeLineWidth;
}
