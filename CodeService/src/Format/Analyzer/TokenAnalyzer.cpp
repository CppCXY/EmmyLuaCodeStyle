#include "CodeService/Format/Analyzer/TokenAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatState.h"
#include "CodeService/Format/Analyzer/SpaceAnalyzer.h"

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
                default: {
                    break;
                }
            }
        }
    }
}

void TokenAnalyzer::Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        auto it = _tokenStrategies.find(syntaxNode.GetIndex());
        if (it != _tokenStrategies.end()) {
            resolve.SetTokenStrategy(it->second);
        }
    }
}

void TokenAnalyzer::Mark(LuaSyntaxNode &n, const LuaSyntaxTree &t, TokenStrategy strategy) {
    _tokenStrategies[n.GetIndex()] = strategy;
}

bool TokenAnalyzer::IsRemove(LuaSyntaxNode &n, const LuaSyntaxTree &t) const {
    auto it = _tokenStrategies.find(n.GetIndex());
    if(it == _tokenStrategies.end()){
        return false;
    }
    return it->second == TokenStrategy::Remove;
}

void TokenAnalyzer::TableFieldAddSep(FormatState &f, LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    auto sep = n.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
    if (!sep.IsNode(t)) {
        auto lastToken = n.GetLastToken(t);
        if (lastToken.GetTokenKind(t) == TK_SHORT_COMMENT
            || lastToken.GetTokenKind(t) == TK_LONG_COMMENT) {
            lastToken = lastToken.GetPrevToken(t);
        }
        Mark(lastToken, t, TokenStrategy::TableAddSep);
    }
}

void TokenAnalyzer::AnalyzeTableField(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    if (f.GetStyle().table_separator_style == TableSeparatorStyle::Semicolon) {
        auto sep = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
        auto comma = sep.GetChildToken(',', t);
        if (comma.IsToken(t)) {
            Mark(comma, t, TokenStrategy::TableSepSemicolon);
        }
    } else if (f.GetStyle().table_separator_style == TableSeparatorStyle::Comma) {
        auto sep = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
        auto semicolon = sep.GetChildToken(';', t);
        if (semicolon.IsToken(t)) {
            Mark(semicolon, t, TokenStrategy::TableSepComma);
        }
    }

    if (f.GetStyle().trailing_table_separator != TrailingTableSeparator::Keep) {
        auto nextToken = syntaxNode.GetNextToken(t);
        while (nextToken.GetTokenKind(t) == TK_SHORT_COMMENT
               || nextToken.GetTokenKind(t) == TK_LONG_COMMENT) {
            nextToken = nextToken.GetNextToken(t);
        }
        // the last table field
        if (nextToken.GetTokenKind(t) == '}') {
            switch (f.GetStyle().trailing_table_separator) {
                case TrailingTableSeparator::Never: {
                    auto sep = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
                    auto sepToken = sep.GetFirstToken(t);
                    Mark(sepToken, t, TokenStrategy::Remove);
                    break;
                }
                case TrailingTableSeparator::Always: {
                    TableFieldAddSep(f, syntaxNode, t);
                    break;
                }
                case TrailingTableSeparator::Smart: {
                    auto tableFieldList = syntaxNode.GetParent(t);
                    if (tableFieldList.GetStartLine(t) == nextToken.GetStartLine(t)) {
                        auto sep = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldSep, t);
                        auto sepToken = sep.GetFirstToken(t);
                        Mark(sepToken, t, TokenStrategy::Remove);
                    } else {
                        TableFieldAddSep(f, syntaxNode, t);
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

bool IsSingleTableOrStringArg(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    for (auto child: children) {
        if (child.GetTokenKind(t) == TK_STRING
            || child.GetTokenKind(t) == TK_LONG_STRING
            || child.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
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

LuaSyntaxNode GetSingleArgStringOrTable(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    for (auto child: children) {
        if (child.GetTokenKind(t) == TK_STRING
            || child.GetTokenKind(t) == TK_LONG_STRING
            || child.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
            return syntaxNode;
        } else if (child.GetSyntaxKind(t) == LuaSyntaxNodeKind::ExpressionList) {
            auto exprs = child.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
            if (exprs.size() == 1) {
                auto expr = exprs.front();
                if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
                    return expr;
                } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::StringLiteralExpression) {
                    auto firstToken = expr.GetFirstToken(t);
                    if (firstToken.GetTokenKind(t) == TK_STRING
                        || firstToken.GetTokenKind(t) == TK_LONG_STRING) {
                        return firstToken;
                    }
                }
            }
        }
    }
    return LuaSyntaxNode(0);
}

void TokenAnalyzer::AnalyzeCallExpression(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    if (IsSingleTableOrStringArg(syntaxNode, t)) {
        switch (f.GetStyle().call_arg_parentheses) {
            case CallArgParentheses::Remove: {
                auto lbrace = syntaxNode.GetChildToken('(', t);
                if(lbrace.IsToken(t)){
                    Mark(lbrace, t, TokenStrategy::Remove);
                    auto rbrace = syntaxNode.GetChildToken(')', t);
                    Mark(rbrace, t, TokenStrategy::Remove);
                }

                break;
            }
            case CallArgParentheses::RemoveStringOnly: {
                auto node = GetSingleArgStringOrTable(syntaxNode, t);
                if (node.GetTokenKind(t) == TK_STRING
                    || node.GetTokenKind(t) == TK_LONG_STRING) {
                    auto lbrace = syntaxNode.GetChildToken('(', t);
                    if(lbrace.IsToken(t)) {
                        Mark(lbrace, t, TokenStrategy::Remove);
                        auto rbrace = syntaxNode.GetChildToken(')', t);
                        Mark(rbrace, t, TokenStrategy::Remove);
                    }
                }

                break;
            }
            case CallArgParentheses::RemoveTableOnly: {
                auto node = GetSingleArgStringOrTable(syntaxNode, t);
                if (node.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
                    auto lbrace = syntaxNode.GetChildToken('(', t);
                    if(lbrace.IsToken(t)) {
                        Mark(lbrace, t, TokenStrategy::Remove);
                        auto rbrace = syntaxNode.GetChildToken(')', t);
                        Mark(rbrace, t, TokenStrategy::Remove);
                    }
                }

                break;
            }
            default: {
                break;
            }
        }

        auto spaceAnalyzer = f.GetAnalyzer<SpaceAnalyzer>();
        if (spaceAnalyzer) {
            switch (f.GetStyle().space_before_function_call_single_arg) {
                case FunctionSingleArgSpace::None: {
                    spaceAnalyzer->SpaceLeft(syntaxNode, t, 0);
                    break;
                }
                case FunctionSingleArgSpace::Always: {
                    spaceAnalyzer->SpaceLeft(syntaxNode, t, 1);
                    break;
                }
                case FunctionSingleArgSpace::OnlyString: {
                    auto firstToken = syntaxNode.GetFirstToken(t);
                    if (firstToken.GetTokenKind(t) == TK_STRING
                        || firstToken.GetTokenKind(t) == TK_LONG_STRING) {
                        spaceAnalyzer->SpaceLeft(syntaxNode, t, 1);
                    } else {
                        spaceAnalyzer->SpaceLeft(syntaxNode, t, 0);
                    }
                    break;
                }
                case FunctionSingleArgSpace::OnlyTable: {
                    auto firstChild = syntaxNode.GetFirstChild(t);
                    if (firstChild.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
                        spaceAnalyzer->SpaceLeft(syntaxNode, t, 1);
                    } else {
                        spaceAnalyzer->SpaceLeft(syntaxNode, t, 0);
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


