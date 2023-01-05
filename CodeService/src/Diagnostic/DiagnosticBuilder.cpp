#include "CodeService/Diagnostic/DiagnosticBuilder.h"
#include "CodeService/Config/LanguageTranslator.h"
#include "Util/format.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

DiagnosticBuilder::DiagnosticBuilder(LuaStyle &style, LuaDiagnosticStyle &diagnosticStyle) {
    _state.SetFormatStyle(style);
    _state.SetDiagnosticStyle(diagnosticStyle);
}

void DiagnosticBuilder::DiagnosticAnalyze(const LuaSyntaxTree &t) {
    _state.Analyze(t);
}

std::vector<LuaDiagnostic> &DiagnosticBuilder::GetDiagnosticResults(const LuaSyntaxTree &t) {
    CodeStyleCheck(t);
    NameStyleCheck(t);
    SpellCheck(t);

    return _diagnostics;
}

void DiagnosticBuilder::PushDiagnostic(DiagnosticType type, TextRange range, std::string_view message) {
    _diagnostics.emplace_back(type, range, message);
}

void DiagnosticBuilder::CodeStyleCheck(const LuaSyntaxTree &t) {
    if (!_state.GetDiagnosticStyle().code_style_check) {
        return;
    }

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
            for (auto &analyzer: _state.GetAnalyzers()) {
                analyzer->Query(_state, traverse.Node, t, resolve);
            }
            auto children = traverse.Node.GetChildren(t);
            // 不采用 <range>
            for (auto rIt = children.rbegin(); rIt != children.rend(); rIt++) {
                traverseStack.emplace_back(*rIt, TraverseEvent::Enter);
            }

            DoDiagnosticResolve(traverse.Node, t, resolve);
        } else {
            traverseStack.pop_back();
            for (auto &analyzer: _state.GetAnalyzers()) {
                analyzer->ExitQuery(_state, traverse.Node, t, resolve);
            }
//            ExitResolve(traverse.Node, t, resolve);

            if (_state.GetCurrentIndent().SyntaxNode.GetIndex() == traverse.Node.GetIndex()) {
                _state.RecoverIndent();
            }
        }
    }

    auto text = root.GetText(t);
    if (_state.GetStyle().insert_final_newline) {
        if (!text.empty()) {
            if (text.back() == '\n' || text.back() == '\r') {
                PushDiagnostic(DiagnosticType::EndWithNewLine,
                               TextRange(text.size(), text.size()),
                               LText("must end with new line")
                );
            }
        }
    } else {
        if (!text.empty()) {
            if (text.back() != '\n' && text.back() != '\r') {
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
}

void DiagnosticBuilder::SpellCheck(const LuaSyntaxTree &t) {
    if (!_state.GetDiagnosticStyle().spell_check) {
        return;
    }
}

void DiagnosticBuilder::DoDiagnosticResolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (resolve.GetIndentStrategy() != IndentStrategy::None) {
        auto indent = resolve.GetIndent();
        if (indent == 0) {
            if (_state.GetStyle().indent_style == IndentStyle::Space) {
                indent = _state.GetStyle().indent_size;
            } else {
                indent = _state.GetStyle().tab_width;
            }
        }

        switch (resolve.GetIndentStrategy()) {
            case IndentStrategy::Relative: {
                _state.AddRelativeIndent(syntaxNode, indent);
                break;
            }
            case IndentStrategy::InvertRelative: {
                _state.AddInvertIndent(syntaxNode, indent);
                break;
            }
            default: {
                break;
            }
        }
    }

    if (syntaxNode.IsToken(t)) {
        auto textRange = syntaxNode.GetTextRange(t);
        auto file = t.GetFile();
        switch (resolve.GetPrevSpaceStrategy()) {
            case PrevSpaceStrategy::AlignPos: {
                auto pos = resolve.GetAlign();
                if (pos != textRange.StartOffset) {
                    PushDiagnostic(DiagnosticType::Align, textRange,
                                   util::format(LText("should align to {}"),
                                                file.GetColumn(pos)
                                   )
                    );
                }
                break;
            }
            case PrevSpaceStrategy::AlignRelativeIndent: {
                auto relativePos = resolve.GetAlign();
                auto indentState = _state.GetCurrentIndent();
                auto pos = relativePos + indentState.SpaceSize + indentState.TabSize * _state.GetStyle().tab_width;
                if (pos != textRange.StartOffset) {
                    PushDiagnostic(DiagnosticType::Align, textRange,
                                   util::format(LText("should align to {}"),
                                                file.GetColumn(pos)
                                   )
                    );
                }
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
                if (nextToken.IsToken(t)) {
                    ProcessSpaceDiagnostic(syntaxNode, nextToken, t, resolve.GetNextSpace());
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

void DiagnosticBuilder::ProcessSpaceDiagnostic(LuaSyntaxNode &node, LuaSyntaxNode &next, const LuaSyntaxTree &t,
                                               size_t shouldSpace) {
    auto leftOffset = node.GetTextRange(t).EndOffset;
    auto rightOffset = next.GetTextRange(t).StartOffset;
    int diff = static_cast<int>(rightOffset - leftOffset) - 1;
    if (diff == shouldSpace) {
        return;
    }
    auto additional = GetAdditionalNote(node, next, t);
    switch (shouldSpace) {
        case 0: {
            PushDiagnostic(DiagnosticType::Space, TextRange(leftOffset, rightOffset),
                           util::format(LText("unnecessary whitespace {}"), additional)
            );
            break;
        }
        case 1: {
            if (diff == 0) {
                PushDiagnostic(DiagnosticType::Space, TextRange(leftOffset, rightOffset + 1),
                               util::format(LText("missing whitespace {}"), additional)
                );
            } else {
                PushDiagnostic(DiagnosticType::Space, TextRange(leftOffset + 1, rightOffset),
                               util::format(LText("multiple spaces {}"), additional)
                );
            }
            break;
        }
        default: {
            if (static_cast<std::size_t>(diff) < shouldSpace) {
                PushDiagnostic(DiagnosticType::Space, TextRange(leftOffset, rightOffset),
                               util::format(LText("expected {} whitespace, found {} {}"),
                                            shouldSpace, diff, additional)
                );
            } else {
                PushDiagnostic(DiagnosticType::Space, TextRange(leftOffset + 1, rightOffset),
                               util::format(LText("expected {} whitespace, found {} {}"),
                                            shouldSpace, diff, additional)
                );
            }
        }
    }

}


