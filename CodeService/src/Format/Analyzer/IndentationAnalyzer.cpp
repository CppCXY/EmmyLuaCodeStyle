#include "CodeService/Format/Analyzer/IndentationAnalyzer.h"
#include "CodeService/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

// 但是我不能这样做
//using enum LuaSyntaxNodeKind;
using NodeKind = LuaSyntaxNodeKind;
using MultiKind = LuaSyntaxMultiKind;

IndentationAnalyzer::IndentationAnalyzer() {
}

void IndentationAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    Indenter(syntaxNode, t);
                    if (f.GetStyle().never_indent_comment_on_if_branch) {
                        auto ifStmt = syntaxNode.GetParent(t);
                        if (ifStmt.GetSyntaxKind(t) == LuaSyntaxNodeKind::IfStatement) {
                            auto ifBranch = syntaxNode.GetNextToken(t);
                            if (ifBranch.GetTokenKind(t) == TK_ELSEIF
                                || ifBranch.GetTokenKind(t) == TK_ELSE) {
                                auto bodyChildren = syntaxNode.GetChildren(t);
                                bool isCommentOnly = true;
                                for (auto bodyChild: bodyChildren) {
                                    if (bodyChild.IsNode(t)) {
                                        isCommentOnly = false;
                                        break;
                                    }
                                }
                                if (isCommentOnly) {
                                    break;
                                }
                                std::size_t siblingLine = ifBranch.GetStartLine(t);
                                for (auto it = bodyChildren.rbegin(); it != bodyChildren.rend(); it++) {
                                    auto n = *it;
                                    if (n.GetTokenKind(t) != TK_SHORT_COMMENT) {
                                        break;
                                    }
                                    auto commentLine = n.GetStartLine(t);
                                    if (commentLine + 1 == siblingLine) {
                                        Indenter(n, t, IndentData(IndentType::InvertIndentation));
                                        siblingLine = commentLine;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ParamList: {
                    Indenter(syntaxNode, t);
                    break;
                }
                case LuaSyntaxNodeKind::CallExpression: {
                    if (syntaxNode.GetChildToken('(', t).IsToken(t)) {
                        auto exprList = syntaxNode.GetChildSyntaxNode(NodeKind::ExpressionList, t);
                        if (exprList.IsNode(t)) {
                            AnalyzeCallExprList(f, exprList, t);
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::LocalStatement:
                case LuaSyntaxNodeKind::AssignStatement:
                case LuaSyntaxNodeKind::ReturnStatement: {
                    auto exprList = syntaxNode.GetChildSyntaxNode(NodeKind::ExpressionList, t);
                    if (exprList.IsNode(t)) {
                        AnalyzeExprList(f, exprList, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::WhileStatement:
                case LuaSyntaxNodeKind::RepeatStatement: {
                    auto expr = syntaxNode.GetChildSyntaxNode(MultiKind::Expression, t);
                    if (expr.IsNode(t)) {
                        Indenter(expr, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::IfStatement: {
                    if (!f.GetStyle().never_indent_before_if_condition) {
                        auto exprs = syntaxNode.GetChildSyntaxNodes(MultiKind::Expression, t);
                        for (auto expr: exprs) {
                            Indenter(expr, t, IndentData(
                                    IndentType::Standard,
                                    f.GetStyle().continuation_indent
                            ));
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ExpressionStatement: {
                    auto suffixedExpression = syntaxNode.GetChildSyntaxNode(NodeKind::SuffixedExpression, t);

                    for (auto expr: suffixedExpression.GetChildren(t)) {
                        if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::IndexExpression) {
                            Indenter(expr, t, IndentData(
                                    IndentType::Standard,
                                    f.GetStyle().continuation_indent
                            ));
                        } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::CallExpression) {
                            auto prevSibling = expr.GetPrevSibling(t);
                            if (prevSibling.GetSyntaxKind(t) != LuaSyntaxNodeKind::NameExpression) {
                                Indenter(expr, t, IndentData(
                                        IndentType::WhenPrevIndent,
                                        f.GetStyle().continuation_indent
                                ));
                            }
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::TableExpression: {
                    auto tableFieldList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldList, t);
                    for (auto field: tableFieldList.GetChildren(t)) {
                        Indenter(field, t, IndentData(
                                IndentType::WhenLineBreak,
                                f.GetStyle().indent_style == IndentStyle::Space ?
                                f.GetStyle().indent_size : f.GetStyle().tab_width
                        ));
                    }

                    break;
                }
                case LuaSyntaxNodeKind::TableField: {
                    if (syntaxNode.GetChildToken('=', t).IsToken(t)) {
                        auto expr = syntaxNode.GetLastChildSyntaxNode(LuaSyntaxMultiKind::Expression, t);
                        AnalyzeTableFieldKeyValuePairExpr(f, expr, t);
                    }
                        // suffix expr
                    else {
                        auto suffixedExpression = syntaxNode.GetChildSyntaxNode(NodeKind::SuffixedExpression, t);

                        for (auto expr: suffixedExpression.GetChildren(t)) {
                            if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::IndexExpression) {
                                Indenter(expr, t, IndentData(
                                        IndentType::Standard,
                                        f.GetStyle().continuation_indent
                                ));
                            } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::CallExpression) {
                                auto prevSibling = expr.GetPrevSibling(t);
                                if (prevSibling.GetSyntaxKind(t) != LuaSyntaxNodeKind::NameExpression) {
                                    Indenter(expr, t, IndentData(
                                            IndentType::WhenPrevIndent,
                                            f.GetStyle().continuation_indent
                                    ));
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
}

void
IndentationAnalyzer::Query(FormatState &f, LuaSyntaxNode &n, const LuaSyntaxTree &t, FormatResolve &resolve) {
    auto it = _indent.find(n.GetIndex());
    if (it != _indent.end()) {
        auto &indentData = it->second;
        switch (indentData.Type) {
            case IndentType::Standard: {
                resolve.SetIndent(indentData.Indent);
                break;
            }
            case IndentType::InvertIndentation: {
                resolve.SetIndent(indentData.Indent, IndentStrategy::InvertRelative);
                break;
            }
            case IndentType::WhenLineBreak: {
                if (f.IsNewLine()) {
                    resolve.SetIndent(indentData.Indent);
                }
                break;
            }
            case IndentType::WhenPrevIndent: {
                auto prev = n.GetPrevSibling(t);
                if (_indentMark.count(prev.GetIndex())) {
                    resolve.SetIndent(indentData.Indent);
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void IndentationAnalyzer::Indenter(LuaSyntaxNode &n, const LuaSyntaxTree &t, IndentData indentData) {
    _indent[n.GetIndex()] = indentData;
}

void IndentationAnalyzer::MarkIndent(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    _indentMark.insert(n.GetIndex());
    auto p = n.GetParent(t);
    while (p.GetSyntaxKind(t) != LuaSyntaxNodeKind::Block
           && !detail::multi_match::StatementMatch(p.GetSyntaxKind(t))
           && !p.IsNull(t)) {
        _indentMark.insert(p.GetIndex());
        p = p.GetParent(t);
    }
}

void IndentationAnalyzer::AnalyzeExprList(FormatState &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t) {
    auto exprs = exprList.GetChildSyntaxNodes(MultiKind::Expression, t);
    if (exprs.size() == 1) {
        auto expr = exprs.front();
        auto syntaxKind = expr.GetSyntaxKind(t);
        // special deal with
        if (syntaxKind == LuaSyntaxNodeKind::TableExpression
            || syntaxKind == LuaSyntaxNodeKind::StringLiteralExpression) {
            return;
        }

        if (!IsExprShouldIndent(expr, t)) {
            return;
        }

    } else {
        bool shouldIndent = false;
        for (auto expr: exprs) {
            if (IsExprShouldIndent(expr, t)) {
                shouldIndent = true;
                break;
            }
        }

        if (!shouldIndent) {
            return;
        }
    }

    Indenter(exprList, t, IndentData(
            IndentType::Standard,
            f.GetStyle().continuation_indent
    ));
}

void IndentationAnalyzer::AnalyzeCallExprList(FormatState &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t) {
    auto exprs = exprList.GetChildSyntaxNodes(MultiKind::Expression, t);

    bool shouldIndent = false;
    for (auto expr: exprs) {
        if (IsExprShouldIndent(expr, t)) {
            shouldIndent = true;
            break;
        }
    }

    if (shouldIndent) {
        Indenter(exprList, t, IndentData(IndentType::Standard));
    }
}

bool IndentationAnalyzer::IsExprShouldIndent(LuaSyntaxNode &expr, const LuaSyntaxTree &t) {
    auto symbolLine = expr.GetPrevToken(t).GetEndLine(t);
    if (expr.GetStartLine(t) != symbolLine) {
        return true;
    }
    // 快速收敛
    if (expr.IsSingleLineNode(t)) {
        return false;
    }

    auto syntaxKind = expr.GetSyntaxKind(t);
    switch (syntaxKind) {
        case LuaSyntaxNodeKind::ClosureExpression:
        case LuaSyntaxNodeKind::TableExpression:
        case LuaSyntaxNodeKind::StringLiteralExpression:
        case LuaSyntaxNodeKind::NameExpression: {
            break;
        }
        case LuaSyntaxNodeKind::SuffixedExpression: {
            auto subExprs = expr.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
            for (auto subExpr: subExprs) {
                if (IsExprShouldIndent(subExpr, t)) {
                    return true;
                }
            }
            break;
        }
        case LuaSyntaxNodeKind::ParExpression:
        case LuaSyntaxNodeKind::UnaryExpression: {
            auto subExpr = expr.GetChildSyntaxNode(LuaSyntaxMultiKind::Expression, t);
            if (subExpr.IsNode(t)) {
                return IsExprShouldIndent(subExpr, t);
            }
            break;
        }
        case LuaSyntaxNodeKind::BinaryExpression: {
            for (auto childNode: expr.GetChildren(t)) {
                if (childNode.IsNode(t) && IsExprShouldIndent(childNode, t)) {
                    return true;
                } else if (childNode.GetStartLine(t) != symbolLine) {
                    return true;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
    return false;
}

void
IndentationAnalyzer::AnalyzeTableFieldKeyValuePairExpr(FormatState &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t) {
    auto syntaxKind = expr.GetSyntaxKind(t);
    // special deal with
    if (syntaxKind == LuaSyntaxNodeKind::TableExpression
        || syntaxKind == LuaSyntaxNodeKind::StringLiteralExpression) {
        return;
    }

    if (IsExprShouldIndent(expr, t)) {
        Indenter(expr, t, IndentData(
                IndentType::Standard,
                f.GetStyle().continuation_indent
        ));
    }
}
