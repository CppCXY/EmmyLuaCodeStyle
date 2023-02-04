#include "CodeService/RangeFormat/RangeFormatBuilder.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

RangeFormatBuilder::RangeFormatBuilder(LuaStyle &style, FormatRange &range)
        : FormatBuilder(style), _validRange(false), _range(range) {

}

std::string RangeFormatBuilder::GetFormatResult(const LuaSyntaxTree &t) {
    _state.Analyze(t);
    auto root = t.GetRootNode();
    std::vector<LuaSyntaxNode> startNodes;
    for (auto child: root.GetChildren(t)) {
        auto childEndLine = child.GetEndLine(t);
        if (childEndLine >= _range.StartLine) {
            startNodes.push_back(child);
        }
        if (childEndLine > _range.EndLine) {
            break;
        }
    }

    LuaSyntaxNode n;
    _state.AddRelativeIndent(n, 0);

    _state.DfsForeach(startNodes, t, [this](LuaSyntaxNode &syntaxNode,
                                            const LuaSyntaxTree &t,
                                            FormatResolve &resolve) {
        CheckRange(syntaxNode, t);
        DoResolve(syntaxNode, t, resolve);
    });

    _validRange = true;
    DealEndWithNewLine(true);
    return _formattedText;
}

FormatRange RangeFormatBuilder::GetReplaceRange() const {
    return _range;
}

void RangeFormatBuilder::WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    if (_validRange) {
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
                _state.GetCurrentWidth() += text.size();
            }
        }
    }
}

void RangeFormatBuilder::WriteSpace(std::size_t space) {
    if (_validRange) {
        return FormatBuilder::WriteSpace(space);
    } else {
        _state.GetCurrentWidth() += space;
    }
}

void RangeFormatBuilder::WriteLine(std::size_t line) {
    if (_validRange) {
        return FormatBuilder::WriteLine(line);
    } else {
        _state.GetCurrentWidth() = 0;
    }
}

void RangeFormatBuilder::WriteIndent() {
    if (_validRange) {
        return FormatBuilder::WriteIndent();
    } else {
        auto topLevelIndent = _state.GetCurrentIndent();
        _state.GetCurrentWidth() += topLevelIndent.SpaceSize + topLevelIndent.TabSize * _state.GetStyle().tab_width;
    }
}

void RangeFormatBuilder::WriteChar(char ch) {
    if (_validRange) {
        return FormatBuilder::WriteChar(ch);
    } else {
        _state.GetCurrentWidth()++;
    }
}

void RangeFormatBuilder::WriteText(std::string_view text) {
    if (_validRange) {
        return FormatBuilder::WriteText(text);
    } else {
        std::size_t last = 0;
        for (std::size_t i = 0; i != text.size(); i++) {
            char ch = text[i];
            if (ch == '\n' || ch == '\r') {
                _state.GetCurrentWidth() = 0;
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
        }
    }
}

void RangeFormatBuilder::CheckRange(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    if (syntaxNode.IsToken(t)) {
        auto tokenEndLine = syntaxNode.GetEndLine(t);
        if (tokenEndLine < _range.StartLine) {
            _validRange = false;
            return;
        }
        auto tokenStartLine = syntaxNode.GetStartLine(t);
        if (tokenStartLine > _range.EndLine) {
            _validRange = false;
            return;
        }

        if (tokenStartLine < _range.StartLine) {
            _range.StartLine = tokenStartLine;
            _range.StartCol = syntaxNode.GetStartCol(t);
        }
        if (tokenEndLine > _range.EndLine) {
            _range.EndLine = tokenEndLine;
        }
        _validRange = true;
    }
}
