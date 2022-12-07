#include "CodeService/Format/Analyzer/TokenAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatBuilder.h"

TokenAnalyzer::TokenAnalyzer() {

}

void TokenAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::TableField: {
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
                                    if (tableFieldList.GetStartLine(t) == syntaxNode.GetStartLine(t)) {
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
                    break;
                }
                case LuaSyntaxNodeKind::CallExpression: {

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

void TokenAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
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

void TokenAnalyzer::TableFieldAddSep(FormatBuilder &f, LuaSyntaxNode &n, const LuaSyntaxTree &t) {
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
