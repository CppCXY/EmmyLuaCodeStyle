#include "CodeFormatCore/Format/Analyzer/TokenAnalyzer.h"
#include "CodeFormatCore/Format/Analyzer/SpaceAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

TokenAnalyzer::TokenAnalyzer() {
}

void TokenAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::TableField: {
                    AnalyzeTableField(f, syntaxNode, t);
                    break;
                }
                case LuaSyntaxNodeKind::CallExpression: {
                    if (f.GetStyle().call_arg_parentheses != CallArgParentheses::Keep) {
                        AnalyzeCallExpression(f, syntaxNode, t);
                    }
                    if (f.GetStyle().remove_call_expression_list_finish_comma) {
                        auto exprList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                        auto last = exprList.GetLastChild(t);
                        if (last.GetTokenKind(t) == ',') {
                            Mark(last, t, TokenStrategy::Remove);
                        }
                    }
                }
                default: {
                    break;
                }
            }
        } else {
            switch (syntaxNode.GetTokenKind(t)) {
                case TK_STRING: {
                    switch (f.GetStyle().quote_style) {
                        case QuoteStyle::Single: {
                            Mark(syntaxNode, t, TokenStrategy::StringSingleQuote);
                            break;
                        }
                        case QuoteStyle::Double: {
                            Mark(syntaxNode, t, TokenStrategy::StringDoubleQuote);
                            break;
                        }
                        default: {
                            break;
                        }
                    }

                    break;
                }
                case TK_SHORT_COMMENT: {
                    if (f.GetStyle().space_after_comment_dash) {
                        AnalyzeComment(f, syntaxNode, t);
                    }
                }
                default: {
                    break;
                }
            }
        }
    }
}

void TokenAnalyzer::Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        auto itPrev = _prevTokenStrategies.find(syntaxNode.GetIndex());
        if (itPrev != _prevTokenStrategies.end()) {
            resolve.SetPrevTokenStrategy(itPrev->second);
        }

        auto it = _tokenStrategies.find(syntaxNode.GetIndex());
        if (it != _tokenStrategies.end()) {
            resolve.SetTokenStrategy(it->second);
        }

        auto itNext = _nextTokenStrategies.find(syntaxNode.GetIndex());
        if (itNext != _nextTokenStrategies.end()) {
            resolve.SetNextTokenStrategy(itNext->second);
        }
    }
}

void TokenAnalyzer::MarkPrev(LuaSyntaxNode n, const LuaSyntaxTree &t, PrevTokenStrategy strategy) {
    _prevTokenStrategies[n.GetIndex()] = strategy;
}

void TokenAnalyzer::Mark(LuaSyntaxNode n, const LuaSyntaxTree &t, TokenStrategy strategy) {
    _tokenStrategies[n.GetIndex()] = strategy;
}

void TokenAnalyzer::MarkNext(LuaSyntaxNode n, const LuaSyntaxTree &t, NextTokenStrategy strategy) {
    _nextTokenStrategies[n.GetIndex()] = strategy;
}

bool TokenAnalyzer::IsRemove(LuaSyntaxNode n, const LuaSyntaxTree &t) const {
    auto it = _tokenStrategies.find(n.GetIndex());
    if (it == _tokenStrategies.end()) {
        return false;
    }
    return it->second == TokenStrategy::Remove;
}

void TokenAnalyzer::TableFieldAddSep(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
    if (!sep.IsNode(t)) {
        auto lastToken = n.GetLastToken(t);
        if (lastToken.GetTokenKind(t) == TK_SHORT_COMMENT || lastToken.GetTokenKind(t) == TK_LONG_COMMENT) {
            lastToken = lastToken.GetPrevToken(t);
        }
        switch (f.GetStyle().table_separator_style) {
            case TableSeparatorStyle::Semicolon: {
                return MarkNext(lastToken, t, NextTokenStrategy::TableAddColon);
            }
            case TableSeparatorStyle::OnlyKVColon: {
                if (n.GetChildToken('=', t).IsToken(t)) {
                    return MarkNext(lastToken, t, NextTokenStrategy::TableAddColon);
                }
                // fallthrough
            }
            default: {
                return MarkNext(lastToken, t, NextTokenStrategy::TableAddComma);
            }
        }
    }
}

void TokenAnalyzer::AnalyzeTableField(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t) {
    if (f.GetStyle().table_separator_style == TableSeparatorStyle::Semicolon) {
        auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
        auto comma = sep.GetChildToken(',', t);
        if (comma.IsToken(t)) {
            Mark(comma, t, TokenStrategy::TableSepSemicolon);
        }
    } else if (f.GetStyle().table_separator_style == TableSeparatorStyle::Comma) {
        auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
        auto semicolon = sep.GetChildToken(';', t);
        if (semicolon.IsToken(t)) {
            Mark(semicolon, t, TokenStrategy::TableSepComma);
        }
    } else if (f.GetStyle().table_separator_style == TableSeparatorStyle::OnlyKVColon) {
        if (n.GetChildToken('=', t).IsToken(t)) {
            auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
            auto semicolon = sep.GetChildToken(',', t);
            if (semicolon.IsToken(t)) {
                Mark(semicolon, t, TokenStrategy::TableSepSemicolon);
            }
        } else {
            auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
            auto semicolon = sep.GetChildToken(';', t);
            if (semicolon.IsToken(t)) {
                Mark(semicolon, t, TokenStrategy::TableSepComma);
            }
        }
    }

    if (f.GetStyle().trailing_table_separator != TrailingTableSeparator::Keep) {
        auto nextToken = n.GetNextTokenSkipComment(t);
        // the last table field
        if (nextToken.GetTokenKind(t) == '}') {
            switch (f.GetStyle().trailing_table_separator) {
                case TrailingTableSeparator::Never: {
                    auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
                    auto sepToken = sep.GetFirstToken(t);
                    Mark(sepToken, t, TokenStrategy::Remove);
                    break;
                }
                case TrailingTableSeparator::Always: {
                    TableFieldAddSep(f, n, t);
                    break;
                }
                case TrailingTableSeparator::Smart: {
                    auto tableFieldList = n.GetParent(t);
                    if (tableFieldList.GetEndLine(t) == nextToken.GetStartLine(t)) {
                        auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
                        auto sepToken = sep.GetFirstToken(t);
                        Mark(sepToken, t, TokenStrategy::Remove);
                    } else {
                        TableFieldAddSep(f, n, t);
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

bool IsSingleTableOrStringArg(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto children = n.GetChildren(t);
    for (auto child: children) {
        if (child.GetSyntaxKind(t) == LuaSyntaxNodeKind::StringLiteralExpression ||
            child.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
            return true;
        } else if (
                child.GetSyntaxKind(t) == LuaSyntaxNodeKind::ExpressionList) {
            auto exprs = child.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
            if (exprs.size() == 1) {
                auto expr = exprs.front();
                if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
                    return true;
                } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::StringLiteralExpression) {
                    return true;
                }
            }
        }
    }
    return false;
}

LuaSyntaxNode GetSingleArgStringOrTable(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto children = n.GetChildren(t);
    for (auto child: children) {
        if (child.GetSyntaxKind(t) == LuaSyntaxNodeKind::StringLiteralExpression ||
            child.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
            return child;
        } else if (child.GetSyntaxKind(t) == LuaSyntaxNodeKind::ExpressionList) {
            auto exprs = child.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
            if (exprs.size() == 1) {
                auto expr = exprs.front();
                if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
                    return expr;
                } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::StringLiteralExpression) {
                    auto firstToken = expr.GetFirstToken(t);
                    if (firstToken.GetTokenKind(t) == TK_STRING || firstToken.GetTokenKind(t) == TK_LONG_STRING) {
                        return firstToken;
                    }
                }
            }
        }
    }
    return LuaSyntaxNode(0);
}

void TokenAnalyzer::AnalyzeCallExpression(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t) {
    if (IsSingleTableOrStringArg(n, t)) {
        switch (f.GetStyle().call_arg_parentheses) {
            case CallArgParentheses::Remove: {
                auto lbrace = n.GetChildToken('(', t);
                if (lbrace.IsToken(t)) {
                    Mark(lbrace, t, TokenStrategy::Remove);
                    auto rbrace = n.GetChildToken(')', t);
                    Mark(rbrace, t, TokenStrategy::Remove);
                }

                break;
            }
            case CallArgParentheses::RemoveStringOnly: {
                auto node = GetSingleArgStringOrTable(n, t);
                if (node.GetTokenKind(t) == TK_STRING || node.GetTokenKind(t) == TK_LONG_STRING) {
                    auto lbrace = n.GetChildToken('(', t);
                    if (lbrace.IsToken(t)) {
                        Mark(lbrace, t, TokenStrategy::Remove);
                        auto rbrace = n.GetChildToken(')', t);
                        Mark(rbrace, t, TokenStrategy::Remove);
                    }
                }

                break;
            }
            case CallArgParentheses::RemoveTableOnly: {
                auto node = GetSingleArgStringOrTable(n, t);
                if (node.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
                    auto lbrace = n.GetChildToken('(', t);
                    if (lbrace.IsToken(t)) {
                        Mark(lbrace, t, TokenStrategy::Remove);
                        auto rbrace = n.GetChildToken(')', t);
                        Mark(rbrace, t, TokenStrategy::Remove);
                    }
                }

                break;
            }
            case CallArgParentheses::Always: {
                auto lbrace = n.GetChildToken('(', t);
                auto spaceAnalyzer = f.GetAnalyzer<SpaceAnalyzer>();
                if (!lbrace.IsToken(t) && spaceAnalyzer) {
                    auto node = GetSingleArgStringOrTable(n, t);
                    if (node.IsToken(t)) {
                        MarkPrev(node, t, PrevTokenStrategy::LeftParentheses);
                        MarkNext(node, t, NextTokenStrategy::RightParentheses);
                        spaceAnalyzer->SpaceAround(node, t, 0, SpaceAnalyzer::SpacePriority::First);
                    } else if (node.GetSyntaxKind(t) == LuaSyntaxNodeKind::StringLiteralExpression) {
                        auto firstToken = node.GetFirstToken(t);
                        MarkPrev(firstToken, t, PrevTokenStrategy::LeftParentheses);
                        MarkNext(firstToken, t, NextTokenStrategy::RightParentheses);
                        spaceAnalyzer->SpaceLeft(node.GetFirstToken(t), t, 0, SpaceAnalyzer::SpacePriority::First);
                    } else {
                        MarkPrev(node.GetFirstToken(t), t, PrevTokenStrategy::LeftParentheses);
                        MarkNext(node.GetLastToken(t), t, NextTokenStrategy::RightParentheses);
                        spaceAnalyzer->SpaceLeft(node.GetFirstToken(t), t, 0, SpaceAnalyzer::SpacePriority::First);
                    }

                    return;
                }

                break;
            }
            default: {
                break;
            }
        }

        auto spaceAnalyzer = f.GetAnalyzer<SpaceAnalyzer>();
        if (spaceAnalyzer) {
            spaceAnalyzer->FunctionCallSingleArgSpace(f, n, t);
        }
    }
}

void TokenAnalyzer::AnalyzeComment(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto text = n.GetText(t);
    std::size_t pos = 0;
    while (pos < text.size() && text[pos] == '-') {
        pos++;
    }
    if (pos == 2 || pos == 3) {
        if (pos < text.size() && text[pos] != ' ') {
            Mark(n, t, TokenStrategy::SpaceAfterCommentDash);
        }
    }
}


