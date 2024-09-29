#include "CodeFormatCore/Format/Analyzer/SemicolonAnalyzer.h"
#include "CodeFormatCore/Config/LuaStyleEnum.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

SemicolonAnalyzer::SemicolonAnalyzer() {
}

void SemicolonAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    if (f.GetStyle().end_statement_with_semicolon == EndStmtWithSemicolon::Keep) {
        return;// No analysis needed
    }

    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            if (detail::multi_match::StatementMatch(syntaxNode.GetSyntaxKind(t))) {
                switch (f.GetStyle().end_statement_with_semicolon) {
                    case EndStmtWithSemicolon::Always: {
                        if (syntaxNode.GetSyntaxKind(t) == LuaSyntaxNodeKind::LabelStatement) {
                            break;// labels should not end with semicolons
                        }
                        if (!EndsWithSemicolon(syntaxNode, t)) {
                            AddSemicolon(syntaxNode, t);
                        }
                        break;
                    }
                    case EndStmtWithSemicolon::ReplaceWithNewline: {
                        // no action needed when there's no semicolons at all!
                        if (ContainsSemicolon(syntaxNode, t)) {
                            if (EndsWithSemicolon(syntaxNode, t)) {
                                RemoveSemicolon(syntaxNode, t);
                                InsertNewLineBeforeNextNode(syntaxNode, t);
                            }
                        }
                        break;
                    }
                    case EndStmtWithSemicolon::SameLine: {
                        if (EndsWithSemicolon(syntaxNode, t)) {
                            if (IsLastStmtOfLine(syntaxNode, t)) {
                                RemoveSemicolon(syntaxNode, t);
                            }
                        }
                        break;
                    }
                    case EndStmtWithSemicolon::Never: {
                        if (CanCorrectRemoveSemicolon(syntaxNode, t)) {
                            RemoveSemicolon(syntaxNode, t);
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
    }
}

void SemicolonAnalyzer::Query(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t, FormatResolve &resolve) {
    auto it = _semicolon.find(n.GetIndex());
    if (it != _semicolon.end()) {
        auto &strategy = it->second;
        switch (strategy) {
            case SemicolonStrategy::Add: {
                resolve.SetNextTokenStrategy(NextTokenStrategy::StmtEndSemicolon);
                break;
            }
            case SemicolonStrategy::Remove: {
                resolve.SetTokenStrategy(TokenStrategy::StmtEndSemicolon);
                break;
            }
            case SemicolonStrategy::InsertNewLine: {
                resolve.SetTokenStrategy(TokenStrategy::NewLineBeforeToken);
                break;
            }
            default: {
                break;
            }
        }
    }
}

void SemicolonAnalyzer::AddSemicolon(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto token = GetLastNonCommentToken(n, t);
    if (token.IsToken(t)) {
        _semicolon[token.GetIndex()] = SemicolonStrategy::Add;
    }
}

void SemicolonAnalyzer::InsertNewLineBeforeNextNode(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto token = n.GetNextTokenSkipComment(t);
    if (token.IsToken(t) && token.GetStartLine(t) == n.GetEndLine(t)) {
        _semicolon[token.GetIndex()] = SemicolonStrategy::InsertNewLine;
    }
}

void SemicolonAnalyzer::RemoveSemicolon(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto token = GetLastNonCommentToken(n, t);
    if (token.IsToken(t)) {
        _semicolon[token.GetIndex()] = SemicolonStrategy::Remove;
    }
}

bool SemicolonAnalyzer::IsLastStmtOfLine(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    // check if next stmt starts on same line as the current one ends
    auto currentEnd = n.GetEndLine(t);
    auto nextToken = n.GetNextToken(t);
    if (!nextToken.IsNull(t)) {
        auto nextStart = nextToken.GetStartLine(t);
        return currentEnd != nextStart;
    }
    return true;// there's no next token
}

bool SemicolonAnalyzer::EndsWithSemicolon(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto token = GetLastNonCommentToken(n, t);
    return token.GetTokenKind(t) == ';';
}

bool SemicolonAnalyzer::ContainsSemicolon(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    return n.GetChildToken(';', t).IsToken(t);
}

LuaSyntaxNode SemicolonAnalyzer::GetLastNonCommentToken(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto token = n.GetLastToken(t);
    switch (token.GetTokenKind(t)) {
        case TK_SHORT_COMMENT:
        case TK_LONG_COMMENT:
        case TK_SHEBANG: {
            return token.GetPrevToken(t);
        }
        default: {
            return token;
        }
    }
}

bool SemicolonAnalyzer::CanCorrectRemoveSemicolon(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    if(!EndsWithSemicolon(n, t)){
        return false;
    }
    auto semicolon = GetLastNonCommentToken(n, t);
    auto nextToken = semicolon.GetNextToken(t);
    if(n.GetEndLine(t) == nextToken.GetStartLine(t)){
        return false;
    }

    auto nextTokenKind = nextToken.GetTokenKind(t);
    if (nextTokenKind == '{' || nextTokenKind == '(') {
        return false;
    }

    return true;
}
