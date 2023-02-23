#include "CodeService/Diagnostic/DiagnosticBuilder.h"
#include "CodeService/Config/LanguageTranslator.h"
#include "Util/format.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

DiagnosticBuilder::DiagnosticBuilder(LuaStyle &style, LuaDiagnosticStyle &diagnosticStyle) {
    _state.SetFormatStyle(style);
    _state.SetDiagnosticStyle(diagnosticStyle);
}

std::vector<LuaDiagnostic> DiagnosticBuilder::GetDiagnosticResults(const LuaSyntaxTree &t) {
    for (auto &d: _nextDiagnosticMap) {
        _diagnostics.push_back(d.second);
    }

    return _diagnostics;
}

void
DiagnosticBuilder::PushDiagnostic(DiagnosticType type,
                                  std::size_t leftIndex,
                                  TextRange range,
                                  std::string_view message,
                                  std::string_view data) {
    _nextDiagnosticMap[leftIndex] = LuaDiagnostic(type, range, message, data);
}

void DiagnosticBuilder::PushDiagnostic(DiagnosticType type, TextRange range, std::string_view message,
                                       std::string_view data) {
    _diagnostics.emplace_back(type, range, message, data);
}

void DiagnosticBuilder::CodeStyleCheck(const LuaSyntaxTree &t) {
    if (!_state.GetDiagnosticStyle().code_style_check) {
        return;
    }

    _state.Analyze(t);
    auto root = t.GetRootNode();
    std::vector<LuaSyntaxNode> startNodes = {root};

    _state.DfsForeach(startNodes, t, [this](LuaSyntaxNode &syntaxNode,
                                            const LuaSyntaxTree &t,
                                            FormatResolve &resolve) {
        DoDiagnosticResolve(syntaxNode, t, resolve);
    });

    auto &file = t.GetFile();
    auto text = file.GetSource();
    if (_state.GetStyle().insert_final_newline) {
        if (!text.empty()) {
            if (text.back() != '\n' && text.back() != '\r') {
                PushDiagnostic(DiagnosticType::EndWithNewLine,
                               TextRange(text.size(), text.size()),
                               LText("must end with new line")
                );
            }
        }
    } else {
        if (!text.empty()) {
            if (text.back() == '\n' || text.back() == '\r') {
                PushDiagnostic(DiagnosticType::EndWithNewLine,
                               TextRange(text.size(), text.size()),
                               LText("can not end with new line")
                );
            }
        }
    }
}

void DiagnosticBuilder::NameStyleCheck(const LuaSyntaxTree &t) {
    if (!_state.GetDiagnosticStyle().name_style_check) {
        return;
    }

    NameStyleChecker nameStyleChecker;
    nameStyleChecker.Analyze(*this, t);
}

void DiagnosticBuilder::SpellCheck(const LuaSyntaxTree &t, CodeSpellChecker &spellChecker) {
    if (!_state.GetDiagnosticStyle().spell_check) {
        return;
    }

    spellChecker.Analyze(*this, t);
}

void DiagnosticBuilder::DoDiagnosticResolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        auto textRange = syntaxNode.GetTextRange(t);
//        auto prevToken = syntaxNode.GetPrevToken(t);
//        if (prevToken.GetEndLine(t) != syntaxNode.GetStartLine(t)) {
//            auto &file = t.GetFile();
//            auto indent = _state.GetCurrentIndent();
//            auto currentIndent = file.GetIndentString(textRange.StartOffset);
//
//        }

        switch (resolve.GetPrevSpaceStrategy()) {
            case PrevSpaceStrategy::AlignPos: {
                ClearDiagnostic(syntaxNode.GetPrevToken(t).GetIndex());
                break;
            }
            case PrevSpaceStrategy::AlignRelativeIndent: {
                ClearDiagnostic(syntaxNode.GetPrevToken(t).GetIndex());
                break;
            }
            default: {
                break;
            }
        }

        switch (resolve.GetTokenStrategy()) {
            case TokenStrategy::StringSingleQuote: {
                if (syntaxNode.GetTokenKind(t) == TK_STRING) {
                    auto text = syntaxNode.GetText(t);
                    if (text.size() >= 2
                        && text.front() == '\"') {
                        PushDiagnostic(DiagnosticType::StringQuote, textRange,
                                       LText("\" should be \' ")
                        );
                        break;
                    }
                }
                break;
            }
            case TokenStrategy::StringDoubleQuote: {
                if (syntaxNode.GetTokenKind(t) == TK_STRING) {
                    auto text = syntaxNode.GetText(t);
                    if (text.size() >= 2
                        && text.front() == '\'') {
                        PushDiagnostic(DiagnosticType::StringQuote, textRange,
                                       LText("\' should be \" ")
                        );
                        break;
                    }
                }

                break;
            }
            default: {
                break;
            }
        }

        switch (resolve.GetNextSpaceStrategy()) {
            case NextSpaceStrategy::Space: {
                auto nextToken = syntaxNode.GetNextToken(t);
                if (nextToken.IsToken(t)
                    && nextToken.GetStartLine(t) == syntaxNode.GetEndLine(t)) {
                    ProcessSpaceDiagnostic(syntaxNode, nextToken, resolve.GetNextSpace(), t);
                }
                break;
            }
            case NextSpaceStrategy::LineBreak: {
//                auto lineSpace = resolve.GetNextLine();
//                switch (lineSpace.Type) {
//                    case LineSpaceType::Fixed: {
//                        WriteLine(lineSpace.Space);
//                        break;
//                    }
//                    case LineSpaceType::Max: {
//                        auto nextToken = syntaxNode.GetNextToken(t);
//                        if (nextToken.IsToken(t)) {
//                            auto currentLine = syntaxNode.GetEndLine(t);
//                            auto nextLine = nextToken.GetStartLine(t);
//                            if (nextLine > currentLine) {
//                                auto line = nextLine - currentLine;
//                                if (line > lineSpace.Space) {
//                                    line = lineSpace.Space;
//                                }
//                                WriteLine(line);
//                                return;
//                            }
//                        }
//                        break;
//                    }
//                    case LineSpaceType::Min: {
//                        auto nextToken = syntaxNode.GetNextToken(t);
//                        if (nextToken.IsToken(t)) {
//                            auto currentLine = syntaxNode.GetEndLine(t);
//                            auto nextLine = nextToken.GetStartLine(t);
//                            if (nextLine > currentLine) {
//                                auto line = nextLine - currentLine;
//                                if (line < lineSpace.Space) {
//                                    line = lineSpace.Space;
//                                }
//                                WriteLine(line);
//                                return;
//                            }
//                        }
//                    }
//                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

std::string DiagnosticBuilder::GetAdditionalNote(LuaSyntaxNode &left, LuaSyntaxNode &right, const LuaSyntaxTree &t) {
    switch (left.GetTokenKind(t)) {
        case TK_STRING:
        case TK_LONG_STRING:
        case TK_LONG_COMMENT:
        case TK_SHORT_COMMENT:
        case TK_NAME: {
            break;
        }
        default: {
            return util::format("after token '{}'", left.GetText(t));
        }
    }

    switch (right.GetTokenKind(t)) {
        case TK_STRING:
        case TK_LONG_STRING:
        case TK_LONG_COMMENT:
        case TK_SHORT_COMMENT:
        case TK_NAME: {
            return "";
        }
        default: {
            return util::format("before token '{}'", right.GetText(t));
        }
    }
}

void DiagnosticBuilder::ProcessSpaceDiagnostic(LuaSyntaxNode &node, LuaSyntaxNode &next, size_t shouldSpace,
                                               const LuaSyntaxTree &t) {
    auto leftOffset = node.GetTextRange(t).EndOffset;
    auto rightOffset = next.GetTextRange(t).StartOffset;
    int diff = static_cast<int>(rightOffset - leftOffset) - 1;
    if (diff == static_cast<int>(shouldSpace)) {
        return;
    }
    auto additional = GetAdditionalNote(node, next, t);
    switch (shouldSpace) {
        case 0: {
            PushDiagnostic(DiagnosticType::Space,
                           node.GetIndex(),
                           TextRange(leftOffset, rightOffset - 1),
                           util::format(LText("unnecessary whitespace {}"), additional)
            );
            break;
        }
        case 1: {
            if (diff == 0) {
                PushDiagnostic(DiagnosticType::Space,
                               node.GetIndex(),
                               TextRange(leftOffset, rightOffset + 1),
                               util::format(LText("missing whitespace {}"), additional)
                );
            } else {
                PushDiagnostic(DiagnosticType::Space,
                               node.GetIndex(),
                               TextRange(leftOffset + 1, rightOffset - 1),
                               util::format(LText("multiple spaces {}"), additional)
                );
            }
            break;
        }
        default: {
            if (static_cast<std::size_t>(diff) < shouldSpace) {
                PushDiagnostic(DiagnosticType::Space, node.GetIndex(),
                               TextRange(leftOffset, rightOffset),
                               util::format(LText("expected {} whitespace, found {} {}"),
                                            shouldSpace, diff, additional)
                );
            } else {
                PushDiagnostic(DiagnosticType::Space, node.GetIndex(),
                               TextRange(leftOffset + 1, rightOffset - 1),
                               util::format(LText("expected {} whitespace, found {} {}"),
                                            shouldSpace, diff, additional)
                );
            }
        }
    }

}

void DiagnosticBuilder::ClearDiagnostic(std::size_t leftIndex) {
    auto it = _nextDiagnosticMap.find(leftIndex);
    if (it != _nextDiagnosticMap.end()) {
        _nextDiagnosticMap.erase(it);
    }
}

FormatState &DiagnosticBuilder::GetState() {
    return _state;
}
