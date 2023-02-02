#include "CodeService/Format/FormatBuilder.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/Analyzer/AlignAnalyzer.h"
#include "CodeService/Format/Analyzer/IndentationAnalyzer.h"


FormatBuilder::FormatBuilder(LuaStyle &style) {
    _state.SetFormatStyle(style);
}

std::string FormatBuilder::GetFormatResult(const LuaSyntaxTree &t) {
    _state.Analyze(t);
    _formattedText.reserve(t.GetFile().GetSource().size());
    auto root = t.GetRootNode();
    std::vector<LuaSyntaxNode> startNodes = {root};

    _state.DfsForeach(startNodes, t, [this](LuaSyntaxNode &syntaxNode,
                                            const LuaSyntaxTree &t,
                                            FormatResolve &resolve) {
        DoResolve(syntaxNode, t, resolve);
    });

    DealEndWithNewLine(_state.GetStyle().insert_final_newline);
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
            _state.GetCurrentWidth() += text.size();
            _formattedText.append(text);
        }
    }
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
    if (syntaxNode.IsToken(t)) {
        if (_state.IsNewLine()) {
            WriteIndent();
            auto indentAnalyzer = _state.GetAnalyzer<IndentationAnalyzer>();
            if (indentAnalyzer) {
                indentAnalyzer->MarkIndent(syntaxNode, t);
            }
        }

        switch (resolve.GetPrevSpaceStrategy()) {
            case PrevSpaceStrategy::AlignPos: {
                auto pos = resolve.GetAlign();
                if (pos > _state.GetCurrentWidth()) {
                    auto space = pos - _state.GetCurrentWidth();
                    WriteSpace(space);
                }
                break;
            }
            case PrevSpaceStrategy::AlignRelativeIndent: {
                auto relativePos = resolve.GetAlign();
                auto indentState = _state.GetCurrentIndent();
                auto pos = relativePos + indentState.SpaceSize + indentState.TabSize * _state.GetStyle().tab_width;
                if (pos > _state.GetCurrentWidth()) {
                    auto space = pos - _state.GetCurrentWidth();
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
                        break;
                    }
                }

                WriteSyntaxNode(syntaxNode, t);
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
                        break;
                    }
                }

                WriteSyntaxNode(syntaxNode, t);
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
                if (_state.GetStyle().table_separator_style == TableSeparatorStyle::Semicolon) {
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
                auto lineSpace = resolve.GetNextLine();
                switch (lineSpace.Type) {
                    case LineSpaceType::Fixed: {
                        WriteLine(lineSpace.Space);
                        break;
                    }
                    case LineSpaceType::Keep: {
                        auto nextToken = syntaxNode.GetNextToken(t);
                        if (nextToken.IsToken(t)) {
                            auto currentLine = syntaxNode.GetEndLine(t);
                            auto nextLine = nextToken.GetStartLine(t);
                            if (nextLine > currentLine) {
                                WriteLine(nextLine - currentLine);
                                return;
                            } else {
                                WriteLine(1);
                            }
                        }
                        break;
                    }
                    case LineSpaceType::Max: {
                        auto nextToken = syntaxNode.GetNextToken(t);
                        if (nextToken.IsToken(t)) {
                            auto currentLine = syntaxNode.GetEndLine(t);
                            auto nextLine = nextToken.GetStartLine(t);
                            if (nextLine > currentLine) {
                                auto line = nextLine - currentLine;
                                if (line > lineSpace.Space) {
                                    line = lineSpace.Space;
                                }
                                WriteLine(line);
                                return;
                            } else {
                                WriteLine(1);
                            }
                        }
                        break;
                    }
                    case LineSpaceType::Min: {
                        auto nextToken = syntaxNode.GetNextToken(t);
                        if (nextToken.IsToken(t)) {
                            auto currentLine = syntaxNode.GetEndLine(t);
                            auto nextLine = nextToken.GetStartLine(t);
                            if (nextLine > currentLine) {
                                auto line = nextLine - currentLine;
                                if (line < lineSpace.Space) {
                                    line = lineSpace.Space;
                                }
                                WriteLine(line);
                                return;
                            } else {
                                WriteLine(1);
                            }
                        }
                    }
                }
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
    _state.GetCurrentWidth() += space;
}

void FormatBuilder::WriteLine(std::size_t line) {
    if (line == 0) {
        return;
    }
    // trim end space
    auto lastIndex = _formattedText.size();
    std::size_t reduce = 0;
    while (_formattedText[lastIndex - reduce - 1] == ' ') {
        reduce++;
        if (lastIndex <= reduce + 1) {
            break;
        }
    }
    if (reduce > 0) {
        _formattedText.resize(_formattedText.size() - reduce);
    }
    auto endOfLine = _state.GetEndOfLine();
    switch (endOfLine) {
        case EndOfLine::CRLF: {
            if (line == 1) {
                _formattedText.push_back('\r');
                _formattedText.push_back('\n');
            } else {
                auto index = _formattedText.size();
                _formattedText.resize(index + 2 * line, '\n');
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
    _state.GetCurrentWidth() = 0;
}

void FormatBuilder::WriteIndent() {
    auto topLevelIndent = _state.GetCurrentIndent();
    switch (_state.GetStyle().indent_style) {
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
    _state.GetCurrentWidth() += topLevelIndent.SpaceSize + topLevelIndent.TabSize * _state.GetStyle().tab_width;
}

void FormatBuilder::WriteChar(char ch) {
    _state.GetCurrentWidth()++;
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
        _state.GetCurrentWidth() += text.size() - last;
        if (last != 0) {
            _formattedText.append(text.substr(last));
        } else {
            _formattedText.append(text);
        }
    }
}

void FormatBuilder::DealEndWithNewLine(bool newLine) {
    if (!_formattedText.empty()) {
        auto endChar = _formattedText.back();
        // trim end new line
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

        if (newLine) {
            WriteLine(1);
        }
    }
}
