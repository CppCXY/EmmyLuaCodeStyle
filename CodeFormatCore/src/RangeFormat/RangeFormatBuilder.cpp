#include "CodeFormatCore/RangeFormat/RangeFormatBuilder.h"
#include "CodeFormatCore/Format/Analyzer/FormatDocAnalyze.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

RangeFormatBuilder::RangeFormatBuilder(LuaStyle &style, FormatRange &range)
    : FormatBuilder(style), _validRange(Valid::Init), _range(range) {
}

std::string RangeFormatBuilder::GetFormatResult(const LuaSyntaxTree &t) {
    _state.Analyze(t);

    auto formatDoc = _state.GetAnalyzer<FormatDocAnalyze>();
    if (formatDoc) {
        auto ignores = formatDoc->GetIgnores();
        for (auto &ignore: ignores) {
            auto ignoreStartLine = LuaSyntaxNode(ignore.StartIndex).GetStartLine(t);
            auto ignoreEndLine = LuaSyntaxNode(ignore.EndIndex).GetEndLine(t);
            if ((_range.StartLine >= ignoreStartLine && _range.StartLine <= ignoreEndLine) || (_range.EndLine >= ignoreStartLine && _range.StartLine < ignoreStartLine)) {
                return "";
            }
        }
    }

    auto root = t.GetRootNode();
    std::vector<LuaSyntaxNode> startNodes = {root};

    _state.DfsForeach(startNodes, t, [this](LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
        CheckRange(syntaxNode, t, resolve);
        DoResolve(syntaxNode, t, resolve);
    });

    _validRange = Valid::Process;
    DealEndWithNewLine(true);
    return _formattedText;
}

FormatRange RangeFormatBuilder::GetReplaceRange() const {
    return _range;
}

void RangeFormatBuilder::WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    if (_validRange == Valid::Process) {
        return FormatBuilder::WriteSyntaxNode(syntaxNode, t);
    } else {
        auto text = syntaxNode.GetText(t);

        switch (syntaxNode.GetTokenKind(t)) {
            case TK_STRING:
            case TK_LONG_STRING:
            case TK_LONG_COMMENT: {
                WriteText(text);
                break;
            }
            default: {
                _state.CurrentWidth() += text.size();
            }
        }
    }
}

void RangeFormatBuilder::WriteSpace(std::size_t space) {
    if (_validRange == Valid::Process) {
        return FormatBuilder::WriteSpace(space);
    } else {
        _state.CurrentWidth() += space;
    }
}

void RangeFormatBuilder::WriteLine(std::size_t line) {
    if (_validRange == Valid::Process) {
        return FormatBuilder::WriteLine(line);
    } else {
        _state.CurrentWidth() = 0;
    }
}

void RangeFormatBuilder::WriteIndent() {
    if (_validRange == Valid::Process) {
        return FormatBuilder::WriteIndent();
    } else {
        auto topLevelIndent = _state.GetCurrentIndent();
        _state.CurrentWidth() += topLevelIndent.SpaceSize + topLevelIndent.TabSize * _state.GetStyle().tab_width;
    }
}

void RangeFormatBuilder::WriteChar(char ch) {
    if (_validRange == Valid::Process) {
        return FormatBuilder::WriteChar(ch);
    } else {
        _state.CurrentWidth()++;
    }
}

void RangeFormatBuilder::WriteText(std::string_view text) {
    if (_validRange == Valid::Process) {
        return FormatBuilder::WriteText(text);
    } else {
        std::size_t last = 0;
        for (std::size_t i = 0; i != text.size(); i++) {
            char ch = text[i];
            if (ch == '\n' || ch == '\r') {
                _state.CurrentWidth() = 0;
                if (ch == '\r' && (i + 1 < text.size()) && (text[i + 1] == '\n')) {
                    i++;
                }
                last = i + 1;
            }
        }

        if (text.size() > last) {
            _state.CurrentWidth() += text.size() - last;
        }
    }
}

void RangeFormatBuilder::CheckRange(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        LuaSyntaxNode startNode = syntaxNode;
        LuaSyntaxNode endNode = syntaxNode;
        if (resolve.GetTokenStrategy() == TokenStrategy::OriginRange) {
            auto r = resolve.GetOriginRange();
            startNode = LuaSyntaxNode(r.StartIndex);
            endNode = LuaSyntaxNode(r.EndIndex);
        }

        switch (_validRange) {
            case Valid::Init: {
                auto tokenEndLine = endNode.GetEndLine(t);
                if (tokenEndLine >= _range.StartLine) {
                    _validRange = Valid::Process;
                    auto tokenStartLine = startNode.GetStartLine(t);
                    if (tokenStartLine < _range.StartLine) {
                        _range.StartLine = tokenStartLine;
                        _range.StartCol = startNode.GetStartCol(t);
                    }
                }
                break;
            }
            case Valid::Process: {
                auto tokenStartLine = startNode.GetStartLine(t);
                if (tokenStartLine > _range.EndLine) {
                    _validRange = Valid::Finish;
                    _state.StopDfsForeach();
                } else {
                    auto tokenEndLine = endNode.GetEndLine(t);
                    if (tokenEndLine > _range.EndLine) {
                        _range.EndLine = tokenEndLine;
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
