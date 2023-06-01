#include "CodeFormatCore/Format/Analyzer/IndentationAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "LuaParser/Parse/LuaParser.h"

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
                    AddIndenter(syntaxNode, t);
                    if (f.GetStyle().never_indent_comment_on_if_branch) {
                        auto ifStmt = syntaxNode.GetParent(t);
                        if (ifStmt.GetSyntaxKind(t) == LuaSyntaxNodeKind::IfStatement) {
                            auto ifBranch = syntaxNode.GetNextToken(t);
                            if (ifBranch.GetTokenKind(t) == TK_ELSEIF || ifBranch.GetTokenKind(t) == TK_ELSE) {
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
                                        AddIndenter(n, t, IndentData(IndentType::InvertIndentation));
                                        siblingLine = commentLine;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ParamList: {
                    AddIndenter(syntaxNode, t);
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
                case LuaSyntaxNodeKind::ParExpression: {
                    auto expr = syntaxNode.GetChildSyntaxNode(MultiKind::Expression, t);
                    if (expr.IsNode(t) && IsExprShouldIndent(expr, t)) {
                        AddIndenter(expr, t);
                    } else {
                        AddIndenter(expr, t, IndentData(IndentType::WhenNewLine));
                    }
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
                        AddIndenter(expr, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::IfStatement: {
                    if (!f.GetStyle().never_indent_before_if_condition) {
                        auto exprs = syntaxNode.GetChildSyntaxNodes(MultiKind::Expression, t);
                        for (auto expr: exprs) {
                            AddIndenter(expr, t, IndentData(IndentType::Standard, f.GetStyle().continuation_indent));
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ExpressionStatement: {
                    auto suffixedExpression = syntaxNode.GetChildSyntaxNode(NodeKind::SuffixedExpression, t);

                    for (auto expr: suffixedExpression.GetChildren(t)) {
                        if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::IndexExpression) {
                            AddIndenter(expr, t, IndentData(IndentType::Standard, f.GetStyle().continuation_indent));
                        } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::CallExpression) {
                            auto prevSibling = expr.GetPrevSibling(t);
                            if (prevSibling.GetSyntaxKind(t) != LuaSyntaxNodeKind::NameExpression) {
                                AddIndenter(expr, t, IndentData(IndentType::WhenPrevIndent, f.GetStyle().continuation_indent));
                            }
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::TableExpression: {
                    auto tableFieldList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldList, t);
                    for (auto field: tableFieldList.GetChildren(t)) {
                        AddIndenter(field, t, IndentData(IndentType::WhenNewLine, f.GetStyle().indent_style == IndentStyle::Space ? f.GetStyle().indent_size : f.GetStyle().tab_width));
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
                                AddIndenter(expr, t, IndentData(IndentType::Standard, f.GetStyle().continuation_indent));
                            } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::CallExpression) {
                                auto prevSibling = expr.GetPrevSibling(t);
                                if (prevSibling.GetSyntaxKind(t) != LuaSyntaxNodeKind::NameExpression) {
                                    AddIndenter(expr, t, IndentData(IndentType::WhenPrevIndent, f.GetStyle().continuation_indent));
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

void IndentationAnalyzer::Query(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t, FormatResolve &resolve) {
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
            case IndentType::WhenNewLine: {
                if (f.IsNewLine(n, t)) {
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

void IndentationAnalyzer::AddIndenter(LuaSyntaxNode n, const LuaSyntaxTree &t, IndentData indentData) {
    _indent[n.GetIndex()] = indentData;
}

void IndentationAnalyzer::AddLinebreakGroup(LuaSyntaxNode parent, std::vector<LuaSyntaxNode> &group, const LuaSyntaxTree &t, std::size_t indent) {
    auto pos = _waitLinebreakGroups.size();
    WaitLinebreakGroup g;
    g.Indent = indent;
    g.TriggerNodes = group;
    g.Parent = parent;
    _waitLinebreakGroups.emplace_back(g);
    for (auto n: group) {
        _waitLinebreak.insert({n.GetIndex(), pos});
    }
}

void IndentationAnalyzer::MarkIndent(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    _indentMark.insert(n.GetIndex());
    auto p = n.GetParent(t);
    while (p.GetSyntaxKind(t) != LuaSyntaxNodeKind::Block && !detail::multi_match::StatementMatch(p.GetSyntaxKind(t)) && !p.IsNull(t)) {
        _indentMark.insert(p.GetIndex());
        p = p.GetParent(t);
    }
}

void IndentationAnalyzer::OnFormatMessage(FormatState &f, FormatEvent event, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t) {
    switch (event) {
        case FormatEvent::NodeExceedLinebreak: {
            ProcessExceedLinebreak(f, syntaxNode, t);
            break;
        }
        default: {
            break;
        }
    }
}

void IndentationAnalyzer::AnalyzeExprList(FormatState &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t) {
    auto exprs = exprList.GetChildSyntaxNodes(MultiKind::Expression, t);
    bool shouldIndent = true;
    if (exprs.size() == 1) {
        auto expr = exprs.front();
        auto syntaxKind = expr.GetSyntaxKind(t);
        // special deal with
        if (syntaxKind == LuaSyntaxNodeKind::TableExpression || syntaxKind == LuaSyntaxNodeKind::StringLiteralExpression) {
            return;
        }

        if (!IsExprShouldIndent(expr, t)) {
            shouldIndent = false;
        }
    } else {
        shouldIndent = false;
        for (auto expr: exprs) {
            if (IsExprShouldIndent(expr, t)) {
                shouldIndent = true;
                break;
            }
        }
    }

    if (shouldIndent) {
        AddIndenter(exprList, t, IndentData(IndentType::Standard, f.GetStyle().continuation_indent));
    } else {
        std::vector<LuaSyntaxNode> group = exprList.GetChildren(t);
        AddLinebreakGroup(exprList, group, t, f.GetStyle().continuation_indent);
    }
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
        AddIndenter(exprList, t, IndentData(IndentType::Standard));
    } else {
        std::vector<LuaSyntaxNode> group = exprList.GetChildren(t);
        AddLinebreakGroup(exprList, group, t, 0);
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
        case LuaSyntaxNodeKind::NameExpression:
        case LuaSyntaxNodeKind::ParExpression: {
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
                } else if (LuaParser::GetBinaryOperator(childNode.GetTokenKind(t)) != BinOpr::OPR_NOBINOPR && childNode.GetPrevToken(t).GetEndLine(t) != childNode.GetStartLine(t)) {
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

void IndentationAnalyzer::AnalyzeTableFieldKeyValuePairExpr(FormatState &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t) {
    auto syntaxKind = expr.GetSyntaxKind(t);
    // special deal with
    if (syntaxKind == LuaSyntaxNodeKind::TableExpression || syntaxKind == LuaSyntaxNodeKind::StringLiteralExpression) {
        return;
    }

    if (IsExprShouldIndent(expr, t)) {
        AddIndenter(expr, t, IndentData(IndentType::Standard, f.GetStyle().continuation_indent));
    }
}

void IndentationAnalyzer::ProcessExceedLinebreak(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t) {
    auto it = _waitLinebreak.find(syntaxNode.GetIndex());
    if (it == _waitLinebreak.end()) {
        return;
    }
    auto pos = it->second;
    if (_waitLinebreakGroups.size() <= pos) {
        return;
    }

    auto &group = _waitLinebreakGroups[pos];
    for (auto n: group.TriggerNodes) {
        _waitLinebreak.erase(n.GetIndex());
    }

    for (auto c: group.Parent.GetChildren(t)) {
        AddIndenter(c, t, IndentData(IndentType::WhenNewLine, group.Indent));
    }
}
