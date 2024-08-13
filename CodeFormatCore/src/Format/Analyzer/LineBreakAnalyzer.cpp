#include "CodeFormatCore/Format/Analyzer/LineBreakAnalyzer.h"
#include "CodeFormatCore/Format/Analyzer/SpaceAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include <algorithm>

using NodeKind = LuaSyntaxNodeKind;

LineBreakAnalyzer::LineBreakAnalyzer() {
}

void LineBreakAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsToken(t)) {
            switch (syntaxNode.GetTokenKind(t)) {
                case TK_SHEBANG:
                case TK_SHORT_COMMENT: {
                    BreakAfter(syntaxNode, t, LineSpace(LineSpaceType::Keep));
                    break;
                }
                case '{':
                case '}': {
                    if (f.GetStyle().break_before_braces) {
                        auto parent = syntaxNode.GetParent(t);
                        if (parent.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression && !parent.IsSingleLineNode(t)) {
                            BreakBefore(syntaxNode, t, LineSpace(LineSpaceType::Keep));
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

void LineBreakAnalyzer::ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    auto children = syntaxNode.GetChildren(t);
                    if (syntaxNode.GetParent(t).IsSingleLineNode(t)) {
                        if (children.size() <= 1) {
                            auto spaceAnalyzer = f.GetAnalyzer<SpaceAnalyzer>();
                            spaceAnalyzer->SpaceAround(syntaxNode, t);
                            continue;
                        }
                    } else if (children.empty()) {
                        auto end = syntaxNode.GetParent(t).GetChildToken(TK_END, t);
                        auto prev = end.GetPrevToken(t);
                        BreakAfter(prev, t, LineSpace(LineSpaceType::Max, 2));
                        continue;
                    }

                    auto &style = f.GetStyle();
                    for (auto stmt: children) {
                        if (stmt.IsNode(t)) {
                            switch (stmt.GetSyntaxKind(t)) {
                                case LuaSyntaxNodeKind::LocalStatement:
                                case LuaSyntaxNodeKind::AssignStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_local_or_assign_statement);
                                    break;
                                }
                                case LuaSyntaxNodeKind::IfStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_if_statement);
                                    break;
                                }
                                case LuaSyntaxNodeKind::DoStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_do_statement);
                                    break;
                                }
                                case LuaSyntaxNodeKind::WhileStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_while_statement);
                                    break;
                                }
                                case LuaSyntaxNodeKind::RepeatStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_repeat_statement);
                                    break;
                                }
                                case LuaSyntaxNodeKind::ForStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_for_statement);
                                    break;
                                }
                                case LuaSyntaxNodeKind::FunctionStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_function_statement);
                                    break;
                                }
                                case LuaSyntaxNodeKind::ExpressionStatement: {
                                    BreakAfter(stmt, t, style.line_space_after_expression_statement);
                                    break;
                                }
                                default: {
                                    BreakAfter(stmt, t, LineSpace(LineSpaceType::Keep));
                                    break;
                                }
                            }

                            if (stmt.GetChildToken(';', t).IsToken(t)) {
                                auto nextStmt = stmt.GetNextSibling(t);
                                if (nextStmt.GetStartLine(t) == stmt.GetEndLine(t)) {
                                    CancelBreakAfter(stmt, t);
                                }
                            }

                        } else {
                            switch (stmt.GetTokenKind(t)) {
                                case TK_LONG_COMMENT:
                                {
                                    auto nextToken = stmt.GetNextToken(t).GetTokenKind(t);
                                    if (nextToken == TK_SHORT_COMMENT) {
                                        break;
                                    }
                                    // Fall through
                                }
                                case TK_SHORT_COMMENT:
                                case TK_SHEBANG: {
                                    BreakAfter(stmt, t, style.line_space_after_comment);
                                    break;
                                }
                                default: {
                                    BreakAfter(stmt, t);
                                }
                            }
                        }
                    }

                    BreakBefore(syntaxNode, t, f.GetStyle().line_space_around_block);
                    BreakAfter(syntaxNode, t, f.GetStyle().line_space_around_block);
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
                case LuaSyntaxNodeKind::IfStatement: {
                    auto exprs = syntaxNode.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
                    for (auto expr: exprs) {
                        AnalyzeConditionExpr(f, expr, t);
                    }
                }
                case LuaSyntaxNodeKind::WhileStatement:
                case LuaSyntaxNodeKind::RepeatStatement: {
                    auto expr = syntaxNode.GetChildSyntaxNode(LuaSyntaxMultiKind::Expression, t);
                    if (expr.IsNode(t)) {
                        AnalyzeConditionExpr(f, expr, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ForStatement: {
                    auto nameList = syntaxNode.GetChildSyntaxNode(NodeKind::NameDefList, t);
                    if (nameList.IsNode(t)) {
                        AnalyzeNameList(f, nameList, t);
                    }
                    auto exprList = syntaxNode.GetChildSyntaxNode(NodeKind::ExpressionList, t);
                    if (exprList.IsNode(t)) {
                        AnalyzeExprList(f, exprList, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::FunctionBody: {
                    auto paramList = syntaxNode.GetChildSyntaxNode(NodeKind::ParamList, t);
                    if (paramList.IsNode(t)) {
                        AnalyzeNameList(f, paramList, t);
                    }

                    break;
                }
                case LuaSyntaxNodeKind::ExpressionStatement: {
                    auto suffixedExpression = syntaxNode.GetChildSyntaxNode(NodeKind::SuffixedExpression, t);
                    if (suffixedExpression.IsNode(t)) {
                        AnalyzeSuffixedExpr(f, suffixedExpression, t);
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

void LineBreakAnalyzer::Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    auto it = _lineBreaks.find(syntaxNode.GetIndex());
    bool collapse = false;
    if (it != _lineBreaks.end()) {
        collapse = it->second.Strategy == LineBreakStrategy::NotBreak;
    }
    if (syntaxNode.IsToken(t) && !collapse) {
        if (resolve.GetNextSpaceStrategy() == NextSpaceStrategy::None || resolve.GetNextSpaceStrategy() == NextSpaceStrategy::Space) {
            auto nextToken = syntaxNode.GetNextToken(t);
            if (nextToken.IsToken(t)) {
                auto currentLine = syntaxNode.GetEndLine(t);
                auto nextLine = nextToken.GetStartLine(t);
                // 既然已经打断那就算了
                if (nextLine > currentLine) {
                    resolve.SetNextLineBreak(LineSpace(nextLine - currentLine));
                }
            }
        }
    }

    // force break
    if (it != _lineBreaks.end()) {
        auto &lineBreakData = it->second;
        switch (lineBreakData.Strategy) {
            case LineBreakStrategy::Standard: {
                resolve.SetNextLineBreak(lineBreakData.Data.Line);
                break;
            }
            case LineBreakStrategy::WhenMayExceed: {
                auto lineWidth = f.CurrentWidth();
                auto &style = f.GetStyle();
                auto relationNode = LuaSyntaxNode(lineBreakData.Data.Index);
                auto guessLineWidth = lineWidth + syntaxNode.GetFirstLineWidth(t) + relationNode.GetFirstLineWidth(t);
                if (guessLineWidth > style.max_line_length) {
                    f.Notify(FormatEvent::NodeExceedLinebreak, syntaxNode, t);
                    resolve.SetNextLineBreak(LineSpace(1, LineBreakReason::ExceedMaxLine));
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void LineBreakAnalyzer::BreakAfter(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t line) {
    auto token = n.GetLastToken(t);
    if (token.IsToken(t)) {
        _lineBreaks[token.GetIndex()] = LineBreakData(line);
    }
}

void LineBreakAnalyzer::BreakAfter(LuaSyntaxNode n, const LuaSyntaxTree &t, LineSpace lineSpace) {
    auto token = n.GetLastToken(t);
    if (token.IsToken(t)) {
        _lineBreaks[token.GetIndex()] = LineBreakData(lineSpace);
    }
}

void LineBreakAnalyzer::BreakBefore(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t line) {
    auto prev = n.GetPrevToken(t);
    if (!prev.IsNull(t)) {
        BreakAfter(prev, t, line);
    }
}

void LineBreakAnalyzer::BreakBefore(LuaSyntaxNode n, const LuaSyntaxTree &t, LineSpace lineSpace) {
    auto prev = n.GetPrevToken(t);
    if (!prev.IsNull(t)) {
        BreakAfter(prev, t, lineSpace);
    }
}

void LineBreakAnalyzer::AnalyzeExprList(FormatState &f, LuaSyntaxNode exprList, const LuaSyntaxTree &t) {
    auto exprs = exprList.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
    if (exprs.empty()) {
        return;
    }
    if (exprs.size() == 1) {
        auto expr = exprs.front();
        AnalyzeExpr(f, expr, t);
    } else {
        for (auto expr: exprs) {
            MarkLazyBreak(expr, t, LineBreakStrategy::WhenMayExceed);
            AnalyzeExpr(f, expr, t);
        }
    }
}

void LineBreakAnalyzer::AnalyzeConditionExpr(FormatState &f, LuaSyntaxNode expr, const LuaSyntaxTree &t) {
    switch (expr.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::BinaryExpression: {
            break;
        }
        default: {
            break;
        }
    }
}

void LineBreakAnalyzer::AnalyzeNameList(FormatState &f, LuaSyntaxNode nameList, const LuaSyntaxTree &t) {
    auto names = nameList.GetChildTokens(TK_NAME, t);
    if (f.GetStyle().auto_collapse_lines && CanCollapseLines(f, nameList, t)) {
        for (auto name: names) {
            MarkNotBreak(name, t);
        }
        return;
    }

    if (f.GetStyle().break_all_list_when_line_exceed && CanBreakAll(f, nameList, t)) {
        for (auto name: names) {
            BreakBefore(name, t);
        }
        return;
    }

    for (auto name: names) {
        MarkLazyBreak(name, t, LineBreakStrategy::WhenMayExceed);
    }
}

void LineBreakAnalyzer::AnalyzeSuffixedExpr(FormatState &f, LuaSyntaxNode expr, const LuaSyntaxTree &t) {
    auto children = expr.GetChildren(t);
    for (auto child: children) {
        AnalyzeExpr(f, child, t);
    }
}

// 是格式化的重点内容
void LineBreakAnalyzer::AnalyzeTableExpr(FormatState &f, LuaSyntaxNode table, const LuaSyntaxTree &t) {

    auto tableFieldList = table.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldList, t);
    auto fields = tableFieldList.GetChildSyntaxNodes(LuaSyntaxNodeKind::TableField, t);

    if (f.GetStyle().auto_collapse_lines && CanCollapseLines(f, tableFieldList, t)) {
        for (auto field: fields) {
            MarkNotBreak(field, t);
        }
        MarkNotBreak(tableFieldList.GetNextToken(t), t);
        return;
    }

    if (f.GetStyle().break_table_list == BreakTableList::Never) {
        return;
    }

    // force break
    bool breakAllList = f.GetStyle().break_all_list_when_line_exceed && CanBreakAll(f, tableFieldList, t);
    if (!breakAllList && f.GetStyle().break_table_list == BreakTableList::Smart) {
        breakAllList = tableFieldList.GetStartLine(t) != tableFieldList.GetEndLine(t) && std::any_of(fields.begin(), fields.end(), [&](LuaSyntaxNode &node) {
                           return node.GetChildToken('=', t).IsToken(t);
                       });
    }
    if (breakAllList) {
        auto leftBrace = table.GetChildToken('{', t);
        if (leftBrace.GetNextSibling(t).GetTokenKind(t) == TK_SHORT_COMMENT || leftBrace.GetNextSibling(t).GetTokenKind(t) == TK_LONG_COMMENT) {
            BreakAfter(leftBrace.GetNextSibling(t), t, LineSpace(LineSpaceType::Keep));
        } else {
            BreakAfter(leftBrace, t, LineSpace(LineSpaceType::Keep));
        }

        for (auto field: fields) {
            BreakAfter(field, t, LineSpace(LineSpaceType::Keep));
        }
    }


    // 如果表的父节点也是表, 那表示格式化的期望就是
    // {
    //      {},
    //      {}
    // }
    if (f.GetStyle().break_table_list == BreakTableList::Smart && table.GetParent(t).GetParent(t).GetSyntaxKind(t) == LuaSyntaxNodeKind::TableFieldList && table.IsSingleLineNode(t)) {
        return;
    }
    for (auto field: fields) {
        auto exprs = field.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
        for (auto expr: exprs) {
            AnalyzeExpr(f, expr, t);
        }
        //        MarkLazyBreak(field, t, LineBreakStrategy::WhenMayExceed);
    }
}

void LineBreakAnalyzer::MarkLazyBreak(LuaSyntaxNode n, const LuaSyntaxTree &t, LineBreakStrategy strategy) {
    auto prevToken = n.GetPrevToken(t);
    if (prevToken.IsToken(t) && !_lineBreaks.count(prevToken.GetIndex())) {
        _lineBreaks[prevToken.GetIndex()] = LineBreakData(strategy, n.GetIndex());
    }
}

void LineBreakAnalyzer::MarkNotBreak(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto prevToken = n.GetPrevToken(t);
    if (prevToken.IsToken(t) && !_lineBreaks.count(prevToken.GetIndex())) {
        _lineBreaks[prevToken.GetIndex()] = LineBreakData(LineBreakStrategy::NotBreak, n.GetIndex());
    }
}

void LineBreakAnalyzer::CancelBreakAfter(LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto token = n.GetLastToken(t);
    if (token.IsToken(t)) {
        _lineBreaks[token.GetIndex()] = LineBreakData(LineBreakStrategy::NotBreak, n.GetIndex());
    }
}

void LineBreakAnalyzer::AnalyzeExpr(FormatState &f, LuaSyntaxNode expr, const LuaSyntaxTree &t) {
    switch (expr.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::BinaryExpression: {
            auto exprs = expr.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
            if (exprs.size() == 2) {
                AnalyzeExpr(f, exprs[0], t);
                MarkLazyBreak(exprs[1], t, LineBreakStrategy::WhenMayExceed);
                return AnalyzeExpr(f, exprs[1], t);
            }
            break;
        }
        case LuaSyntaxNodeKind::ClosureExpression: {
            auto paramList = expr.GetChildSyntaxNode(LuaSyntaxNodeKind::ParamList, t);
            AnalyzeNameList(f, paramList, t);
            break;
        }
        case LuaSyntaxNodeKind::UnaryExpression: {
            auto subExpr = expr.GetChildSyntaxNode(LuaSyntaxMultiKind::Expression, t);
            AnalyzeExpr(f, subExpr, t);
            break;
        }
        case LuaSyntaxNodeKind::TableExpression: {
            AnalyzeTableExpr(f, expr, t);
            break;
        }
        case LuaSyntaxNodeKind::CallExpression: {
            auto exprList = expr.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
            AnalyzeExprList(f, exprList, t);
            break;
        }
        case LuaSyntaxNodeKind::StringLiteralExpression:
        case LuaSyntaxNodeKind::NameExpression:
        case LuaSyntaxNodeKind::IndexExpression: {
            MarkLazyBreak(expr, t, LineBreakStrategy::WhenMayExceed);
            break;
        }
        case LuaSyntaxNodeKind::SuffixedExpression: {
            AnalyzeSuffixedExpr(f, expr, t);
            break;
        }
        default: {
            break;
        }
    }
}

bool LineBreakAnalyzer::CanBreakAll(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t) {
    switch (n.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::ParamList:
        case LuaSyntaxNodeKind::TableFieldList: {
            auto maxLineWidth = f.GetStyle().max_line_length;
            if (n.GetEndCol(t) >= maxLineWidth) {
                return true;
            }
            return !n.IsSingleLineNode(t);
        }
        default: {
            return false;
        }
    }
}

bool LineBreakAnalyzer::CanCollapseLines(FormatState &f, LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    auto maxLineWidth = f.GetStyle().max_line_length;
    auto prevToken = n.GetPrevToken(t);
    if (prevToken.IsNull(t)) {
        return false;
    }
    auto startCol = prevToken.GetEndCol(t);
    switch (n.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::ParamList:
        case LuaSyntaxNodeKind::TableFieldList: {
            auto children = n.GetChildren(t);
            auto lineWidth = startCol;
            for (auto child: children) {
                auto tokenKind = child.GetTokenKind(t);
                if (tokenKind == ',') {
                    lineWidth++;// ', '
                } else if (tokenKind == TK_SHORT_COMMENT || tokenKind == TK_LONG_COMMENT) {
                    return false;// 若有注释禁止塌缩
                }
                lineWidth += child.GetText(t).size();
            }
            return lineWidth <= maxLineWidth;
        }
        default: {
            return false;
        }
    }
}
