#include "CodeService/Format/Analyzer/LineBreakAnalyzer.h"
#include <algorithm>
#include "CodeService/Format/FormatBuilder.h"
#include "CodeService/Format/Analyzer/SpaceAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

using NodeKind = LuaSyntaxNodeKind;

LineBreakAnalyzer::LineBreakAnalyzer() {

}

void LineBreakAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    if (syntaxNode.GetParent(t).IsSingleLineNode(t)) {
                        if (syntaxNode.GetChildren(t).size() == 1) {
                            auto spaceAnalyzer = f.GetAnalyzer<SpaceAnalyzer>();
                            spaceAnalyzer->SpaceAround(syntaxNode, t);
                            continue;
                        }
                    }
                    BreakBefore(syntaxNode, t);
                    BreakAfter(syntaxNode, t);

                    for (auto stmt: syntaxNode.GetChildren(t)) {
                        BreakAfter(stmt, t);
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
                }
            }
        } else {
            switch (syntaxNode.GetTokenKind(t)) {
                case TK_SHEBANG:
                case TK_SHORT_COMMENT: {
                    BreakAfter(syntaxNode, t);
                    break;
                }
            }
        }
    }
}

void
LineBreakAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        if (resolve.GetNextSpaceStrategy() == NextSpaceStrategy::None
            || resolve.GetNextSpaceStrategy() == NextSpaceStrategy::Space) {
            auto nextToken = syntaxNode.GetNextToken(t);
            if (nextToken.IsToken(t)) {
                auto currentLine = syntaxNode.GetEndLine(t);
                auto nextLine = nextToken.GetStartLine(t);
                // 既然已经打断那就算了
                if (nextLine > currentLine) {
                    resolve.SetNextLineBreak(nextLine - currentLine);
                    return;
                }
            }
        }
    }

    // force break
    auto it = _lineBreaks.find(syntaxNode.GetIndex());
    if (it != _lineBreaks.end()) {
        auto &lineBreakData = it->second;
        switch (lineBreakData.Strategy) {
            case LineBreakStrategy::Standard: {
                resolve.SetNextLineBreak(lineBreakData.Data.Line);
                return;
            }
            case LineBreakStrategy::WhenMayExceed: {
                auto lineWidth = f.GetCurrentWidth();
                auto &style = f.GetStyle();
                auto relationNode = LuaSyntaxNode(lineBreakData.Data.Index);
                auto guessLineWidth = lineWidth + syntaxNode.GetFirstLineWidth(t) + relationNode.GetFirstLineWidth(t);
                if (guessLineWidth > style.max_line_length) {
                    resolve.SetNextLineBreak(1);
                    return;
                }
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

void LineBreakAnalyzer::BreakBefore(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t line) {
    auto prev = n.GetPrevSibling(t);
    if (!prev.IsNull(t)) {
        BreakAfter(prev, t, line);
    }
}

void LineBreakAnalyzer::AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t) {
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

void LineBreakAnalyzer::AnalyzeConditionExpr(FormatBuilder &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t) {
    switch (expr.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::BinaryExpression: {
            break;
        }
        default: {
            break;
        }
    }
}

void LineBreakAnalyzer::AnalyzeNameList(FormatBuilder &f, LuaSyntaxNode &nameList, const LuaSyntaxTree &t) {
    auto names = nameList.GetChildTokens(TK_NAME, t);
    for (auto name: names) {
        MarkLazyBreak(name, t, LineBreakStrategy::WhenMayExceed);
    }
}

void LineBreakAnalyzer::AnalyzeSuffixedExpr(FormatBuilder &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t) {
    auto children = expr.GetChildren(t);
    for (auto child: children) {
        AnalyzeExpr(f, child, t);
    }
}

void LineBreakAnalyzer::MarkLazyBreak(LuaSyntaxNode n, const LuaSyntaxTree &t, LineBreakStrategy strategy) {
    auto prevToken = n.GetPrevToken(t);
    if (prevToken.IsToken(t) && !_lineBreaks.contains(prevToken.GetIndex())) {
        _lineBreaks[prevToken.GetIndex()] = LineBreakData(strategy, n.GetIndex());
    }
}

void LineBreakAnalyzer::AnalyzeExpr(FormatBuilder &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t) {
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
            auto tableFieldList = expr.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldList, t);
            auto fields = tableFieldList.GetChildSyntaxNodes(LuaSyntaxNodeKind::TableField, t);
            auto forceBreak =
                    tableFieldList.GetStartLine(t) != tableFieldList.GetEndLine(t)
                    && std::any_of(fields.begin(), fields.end(),
                                   [&](LuaSyntaxNode &node) {
                                       return node.GetChildToken('=', t).IsToken(t);
                                   });

            if (forceBreak) {
                BreakAfter(expr.GetChildToken('{', t), t);
                for (auto field: fields) {
                    BreakAfter(field, t);
                }
            } else {
                for (auto field: fields) {
                    MarkLazyBreak(field, t, LineBreakStrategy::WhenMayExceed);
                }
            }
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
