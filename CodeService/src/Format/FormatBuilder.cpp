#include "CodeService/Format/FormatBuilder.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/Analyzer/AlignAnalyzer.h"
#include "CodeService/Format/Analyzer/TokenAnalyzer.h"

FormatBuilder::FormatBuilder(LuaStyle &style)
        : _style(style),
          _fileEndOfLine(EndOfLine::LF),
          _writeLineWidth(0) {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {
    _fileEndOfLine = t.GetFile().GetEndOfLine();
    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();
    AddAnalyzer<LineBreakAnalyzer>();
    AddAnalyzer<AlignAnalyzer>();
    AddAnalyzer<TokenAnalyzer>();

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

    if (!_formattedText.empty()) {
        auto endChar = _formattedText.back();
        if (_style.insert_final_newline) {
            if (endChar != '\r' && endChar != '\n') {
                WriteLine(1);
            }
        } else {
            if (endChar == '\r' || endChar == '\n') {
                auto lastIndex = _formattedText.size();
                std::size_t reduce = 0;
                while (_formattedText[lastIndex - reduce - 1] == '\r'
                       || _formattedText[lastIndex - reduce - 1] == '\n') {
                    reduce++;
                    if (lastIndex <= reduce + 1) {
                        break;
                    }
                }
                _formattedText.resize(lastIndex - reduce);
            }
        }
    }

    return _formattedText;
}

void FormatBuilder::WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto text = syntaxNode.GetText(t);

    switch (syntaxNode.GetTokenKind(t)) {
        case TK_STRING:
        case TK_LONG_STRING:
        case TK_LONG_COMMENT: {
            WriteText(text);
            break;
        }
        default: {
            _writeLineWidth += text.size();
            _formattedText.append(syntaxNode.GetText(t));
        }
    }
}

void FormatBuilder::AddRelativeIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent) {
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

void FormatBuilder::AddInvertIndent(LuaSyntaxNode &syntaxNoe, std::size_t indent) {
    if (_indentStack.empty()) {
        _indentStack.emplace(syntaxNoe, 0, 0);
        return;
    }
    auto top = _indentStack.top();
    switch (_style.indent_style) {
        case IndentStyle::Space: {
            std::size_t spaceSize = 0;
            if (top.SpaceSize > indent) {
                spaceSize = top.SpaceSize - indent;
            }
            _indentStack.emplace(syntaxNoe, spaceSize, top.TabSize);
            break;
        }
        case IndentStyle::Tab: {
            auto tabIndent = indent / _style.tab_width;
            auto spaceIndent = indent % _style.tab_width;
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

void FormatBuilder::RecoverIndent() {
    if (_indentStack.empty()) {
        return;
    }
    _indentStack.pop();
}

bool ExistDel(char del, std::string_view text) {
    text = text.substr(1, text.size() - 2);
    char ch = '\0';
    for (std::size_t i = 0; i < text.size(); ++i) {
        ch = text[i];
        if (ch == del) {
            return true;
        } else if (ch == '\\') {
            ++i;
        }
    }

    return false;
}

void FormatBuilder::DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (resolve.GetIndentStrategy() != IndentStrategy::None) {
        auto indent = resolve.GetIndent();
        if (indent == 0) {
            if (_style.indent_style == IndentStyle::Space) {
                indent = _style.indent_size;
            } else {
                indent = _style.tab_width;
            }
        }

        switch (resolve.GetIndentStrategy()) {
            case IndentStrategy::Relative: {
                AddRelativeIndent(syntaxNode, indent);
                break;
            }
            case IndentStrategy::InvertRelative: {
                AddInvertIndent(syntaxNode, indent);
                break;
            }
            default: {
                break;
            }
        }
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
                    auto text = syntaxNode.GetText(t);
                    auto del = '\'';
                    if (text.size() >= 2
                        && text.front() == '\"'
                        && !ExistDel(del, text)) {
                        WriteChar(del);
                        WriteText(text.substr(1, text.size() - 2));
                        WriteChar(del);
                    }
                }
                break;
            }
            case TokenStrategy::StringDoubleQuote: {
                if (syntaxNode.GetTokenKind(t) == TK_STRING) {
                    auto text = syntaxNode.GetText(t);
                    auto del = '\"';
                    if (text.size() >= 2
                        && text.front() == '\''
                        && !ExistDel(del, text)) {
                        WriteChar(del);
                        WriteText(text.substr(1, text.size() - 2));
                        WriteChar(del);
                    }
                }
                break;
            }
            case TokenStrategy::TableSepComma: {
                WriteChar(',');
                break;
            }
            case TokenStrategy::TableSepSemicolon: {
                WriteChar(';');
                break;
            }
            case TokenStrategy::TableAddSep: {
                WriteSyntaxNode(syntaxNode, t);
                if (_style.table_separator_style == TableSeparatorStyle::Semicolon) {
                    WriteChar(';');
                } else {
                    WriteChar(',');
                }
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
    auto endOfLine = _style.detect_end_of_line ?
                     _fileEndOfLine : _style.end_of_line;
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

void FormatBuilder::WriteChar(char ch) {
    _writeLineWidth++;
    _formattedText.push_back(ch);
}

void FormatBuilder::WriteText(std::string_view text) {
    std::size_t last = 0;
    for (std::size_t i = 0; i != text.size(); i++) {
        char ch = text[i];
        if (ch == '\n' || ch == '\r') {
            if (last < i) {
                _formattedText.append(text.substr(last, i - last));
            }
            WriteLine(1);
            if (ch == '\r'
                && (i + 1 < text.size())
                && (text[i + 1] == '\n')) {
                i++;
            }
            last = i + 1;
        }
    }

    if (text.size() > last) {
        _writeLineWidth += text.size() - last;
        if (last != 0) {
            _formattedText.append(text.substr(last));
        } else {
            _formattedText.append(text);
        }
    }
}
