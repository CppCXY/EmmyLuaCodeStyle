#include "CodeFormatCore/Diagnostic/CodeStyle/CodeStyleChecker.h"
#include "CodeFormatCore/Config/LanguageTranslator.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"
#include "CodeFormatCore/Format/Analyzer/IndentationAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "Util/format.h"

CodeStyleChecker::CodeStyleChecker() {
}

void CodeStyleChecker::Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t) {
    BasicStyleCheck(d, t);
    EndWithNewLine(d, t);
}

void CodeStyleChecker::BasicStyleCheck(DiagnosticBuilder &d, const LuaSyntaxTree &t) {
    auto &state = d.GetState();
    state.Analyze(t);

    auto root = t.GetRootNode();
    std::vector<LuaSyntaxNode> startNodes = {root};

    state.DfsForeach(startNodes, t, [this, &d](LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
        BasicResolve(syntaxNode, t, resolve, d);
    });
}

void CodeStyleChecker::EndWithNewLine(DiagnosticBuilder &d, const LuaSyntaxTree &t) {
    auto &state = d.GetState();
    auto &file = t.GetFile();
    auto text = file.GetSource();
    if (state.GetStyle().insert_final_newline) {
        if (!text.empty()) {
            if (text.back() != '\n' && text.back() != '\r') {
                d.PushDiagnostic(DiagnosticType::EndWithNewLine,
                                 TextRange(text.size(), 0),
                                 LText("must end with new line"));
            }
        }
    } else {
        if (!text.empty()) {
            if (text.back() == '\n' || text.back() == '\r') {
                d.PushDiagnostic(DiagnosticType::EndWithNewLine,
                                 TextRange(text.size(), 0),
                                 LText("can not end with new line"));
            }
        }
    }
}

void CodeStyleChecker::BasicResolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve,
                                    DiagnosticBuilder &d) {
    if (syntaxNode.IsToken(t)) {
        auto textRange = syntaxNode.GetTextRange(t);
        if (d.GetState().IsNewLine(syntaxNode, t)) {
            ProcessIndentDiagnostic(syntaxNode, t, d);
        }

        switch (resolve.GetPrevSpaceStrategy()) {
            case PrevSpaceStrategy::AlignPos: {
                d.ClearDiagnostic(syntaxNode.GetPrevToken(t).GetIndex());
                break;
            }
            case PrevSpaceStrategy::AlignRelativeIndent: {
                d.ClearDiagnostic(syntaxNode.GetPrevToken(t).GetIndex());
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
                    if (text.size() >= 2 && text.front() == '\"' && !string_util::ExistDel('\'', text)) {
                        d.PushDiagnostic(DiagnosticType::StringQuote, textRange,
                                         LText("\" should be \' "));
                        break;
                    }
                }
                break;
            }
            case TokenStrategy::StringDoubleQuote: {
                if (syntaxNode.GetTokenKind(t) == TK_STRING) {
                    auto text = syntaxNode.GetText(t);
                    if (text.size() >= 2 && text.front() == '\'' && !string_util::ExistDel('\"', text)) {
                        d.PushDiagnostic(DiagnosticType::StringQuote, textRange,
                                         LText("\' should be \" "));
                        break;
                    }
                }

                break;
            }
            case TokenStrategy::StmtEndSemicolon: {
                switch (d.GetState().GetStyle().end_statement_with_semicolon) {
                    case EndStmtWithSemicolon::Never: {
                        d.PushDiagnostic(DiagnosticType::Semicolon, textRange,
                                         LText("expected statement not to end with ;"));
                        break;
                    }
                    case EndStmtWithSemicolon::SameLine: {
                        d.PushDiagnostic(DiagnosticType::Semicolon, textRange,
                                         LText("; should only separate multiple statements on a single line"));
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            default: {
                break;
            }
        }

        switch (resolve.GetTokenAddStrategy()) {
            case TokenAddStrategy::StmtEndSemicolon: {
                d.PushDiagnostic(DiagnosticType::Semicolon,
                                 TextRange(textRange.GetEndOffset(), 1),
                                 LText("expected ; at end of statement"));
                break;
            }
            default: {
                break;
            }
        }

        switch (resolve.GetNextSpaceStrategy()) {
            case NextSpaceStrategy::Space: {
                auto nextToken = syntaxNode.GetNextToken(t);
                if (nextToken.IsToken(t) && nextToken.GetStartLine(t) == syntaxNode.GetEndLine(t)) {
                    ProcessSpaceDiagnostic(syntaxNode, nextToken, resolve.GetNextSpace(), t, d);
                }
                break;
            }
            case NextSpaceStrategy::LineBreak: {
                break;
            }
            default: {
                break;
            }
        }
    }
}

void CodeStyleChecker::ProcessSpaceDiagnostic(LuaSyntaxNode &node, LuaSyntaxNode &next, size_t shouldSpace,
                                              const LuaSyntaxTree &t, DiagnosticBuilder &d) {
    auto leftOffset = node.GetTextRange(t).GetEndOffset();
    auto rightOffset = next.GetTextRange(t).StartOffset;
    if (rightOffset < leftOffset + 1) {
        return;
    }

    std::size_t diff = rightOffset - leftOffset - 1;
    if (diff == shouldSpace) {
        return;
    }
    auto additional = GetAdditionalNote(node, next, t);
    switch (shouldSpace) {
        case 0: {
            d.PushDiagnostic(DiagnosticType::Space,
                             node.GetIndex(),
                             TextRange(leftOffset + 1, diff),
                             util::format(LText("unnecessary whitespace {}"), additional));
            break;
        }
        case 1: {
            if (diff == 0) {
                d.PushDiagnostic(DiagnosticType::Space,
                                 node.GetIndex(),
                                 TextRange(leftOffset, 2),
                                 util::format(LText("missing whitespace {}"), additional));
            } else {
                d.PushDiagnostic(DiagnosticType::Space,
                                 node.GetIndex(),
                                 TextRange(leftOffset + 1, diff),
                                 util::format(LText("multiple spaces {}"), additional));
            }
            break;
        }
        default: {
            if (diff < shouldSpace) {
                d.PushDiagnostic(DiagnosticType::Space, node.GetIndex(),
                                 TextRange(leftOffset, diff + 1),
                                 util::format(LText("expected {} whitespace, found {} {}"),
                                              shouldSpace, diff, additional));
            } else {
                d.PushDiagnostic(DiagnosticType::Space, node.GetIndex(),
                                 TextRange(leftOffset + 1, diff),
                                 util::format(LText("expected {} whitespace, found {} {}"),
                                              shouldSpace, diff, additional));
            }
        }
    }
}

std::string CodeStyleChecker::GetAdditionalNote(LuaSyntaxNode &left, LuaSyntaxNode &right, const LuaSyntaxTree &t) {
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

void CodeStyleChecker::ProcessIndentDiagnostic(LuaSyntaxNode &node, const LuaSyntaxTree &t, DiagnosticBuilder &d) {
    auto textRange = node.GetTextRange(t);
    auto &state = d.GetState();
    auto &file = t.GetFile();

    auto indentAnalyzer = state.GetAnalyzer<IndentationAnalyzer>();
    if (indentAnalyzer) {
        indentAnalyzer->MarkIndent(node, t);
    }

    auto indent = state.GetCurrentIndent();
    auto currentIndentRange = file.GetIndentRange(textRange.StartOffset);
    auto currentIndentString = file.Slice(currentIndentRange);

    struct CheckIndent {
        std::size_t Tab = 0;
        std::size_t Space = 0;
    } checkIndent;

    enum class ParseState {
        Tab,
        Space,
    } parseState = ParseState::Tab;

    for (auto c: currentIndentString) {
        switch (parseState) {
            case ParseState::Tab: {
                if (c == '\t') {
                    checkIndent.Tab++;
                } else {
                    checkIndent.Space++;
                    parseState = ParseState::Space;
                }
                break;
            }
            case ParseState::Space: {
                checkIndent.Space++;
            }
        }
    }

    if (indent.TabSize != checkIndent.Tab || indent.SpaceSize != checkIndent.Space) {
        d.PushDiagnostic(DiagnosticType::Indent,
                         currentIndentRange,
                         util::format("{}, found {} whitespace, {} tab",
                                      GetIndentNote(indent, state.GetStyle().indent_style),
                                      checkIndent.Space, checkIndent.Tab));
    }
}

std::string CodeStyleChecker::GetIndentNote(IndentState indent, IndentStyle style) {
    if (style == IndentStyle::Tab) {
        return util::format("expected {} tab indent", indent.TabSize);
    } else {
        return util::format("expected {} whitespace indent", indent.SpaceSize);
    }
}
