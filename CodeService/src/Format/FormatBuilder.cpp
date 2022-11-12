#include "CodeService/Format/FormatBuilder.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

FormatBuilder::FormatBuilder(LuaStyle &style)
        : _style(style) {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {
    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();
    AddAnalyzer<LineBreakAnalyzer>();

    for (const auto &analyzer: _analyzers) {
        analyzer->Analyze(*this, t);
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
            if (!_indentStack.empty()
                && _indentStack.top().SyntaxNode.GetIndex() == traverse.Node.GetIndex()) {
                RecoverIndent();
            }
        }
    }

    return _formattedText;
}

void FormatBuilder::WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    _formattedText.append(syntaxNode.GetText(t));
}

void FormatBuilder::AddIndent(LuaSyntaxNode &syntaxNoe) {
    if (_indentStack.empty()) {
        _indentStack.push(IndentState(syntaxNoe, 0, 0));
        return;
    }
    auto top = _indentStack.top();
    switch (_style.indent_style) {
        case IndentStyle::Space: {
            _indentStack.push(IndentState(syntaxNoe, top.SpaceSize + _style.indent_size, top.TabSize));
            break;
        }
        case IndentStyle::Tab: {
            _indentStack.push(IndentState(syntaxNoe, top.SpaceSize, top.TabSize + 1));
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
    if (!_formattedText.empty()) {
        auto lastChar = _formattedText.back();
        if (lastChar == '\n' || lastChar == '\r') {
            WriteIndent();
        }
    }

    if (syntaxNode.IsToken(t)) {
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
    }
    switch (resolve.GetSpaceStrategy()) {
        case SpaceStrategy::Space: {
            WriteSpace(resolve.GetNextSpace());
            break;
        }
        case SpaceStrategy::LineBreak: {
            WriteLine(resolve.GetNextLine());
            break;
        }
        case SpaceStrategy::Indent: {
            AddIndent(syntaxNode);
            break;
        }
        default: {
            break;
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
}

const LuaStyle &FormatBuilder::GetStyle() const {
    return _style;
}


