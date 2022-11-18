#include "CodeService/Format/Analyzer/IndentationAnalyzer.h"
#include "CodeService/Format/FormatBuilder.h"

// 但是我不能这样做
//using enum LuaSyntaxNodeKind;
using NodeKind = LuaSyntaxNodeKind;
using MultiKind = LuaSyntaxMultiKind;

IndentationAnalyzer::IndentationAnalyzer() {
}

void IndentationAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    Indenter(syntaxNode, t);
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
                            for (auto expr: exprList.GetChildren(t)) {
                                Indenter(expr, t, IndentData(IndentStrategy::WhenLineBreak));
                            }
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
                    auto exprs = syntaxNode.GetChildSyntaxNodes(MultiKind::Expression, t);
                    for (auto expr: exprs) {
                        Indenter(expr, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ExpressionStatement: {
                    auto suffixedExpression = syntaxNode.GetChildSyntaxNode(NodeKind::SuffixedExpression, t);
                    for (auto expr: suffixedExpression.GetChildren(t)) {
                        if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::IndexExpression) {
                            Indenter(expr, t);
                        } else if (expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::CallExpression) {
                            Indenter(expr, t, IndentData(IndentStrategy::WhenPrevIndent));
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::TableExpression: {
//                    auto tableField = syntaxNode.GetChildSyntaxNode()
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void IndentationAnalyzer::AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t) {
    auto exprs = exprList.GetChildSyntaxNodes(MultiKind::Expression, t);
    if (exprs.size() == 1) {
        auto expr = exprs.front();
        auto syntaxKind = expr.GetSyntaxKind(t);
        if (syntaxKind == LuaSyntaxNodeKind::ClosureExpression
            || syntaxKind == LuaSyntaxNodeKind::TableExpression
            || syntaxKind == LuaSyntaxNodeKind::StringLiteralExpression) {
            return;
        }
    }
    Indenter(exprList, t);
}

void
IndentationAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &n, const LuaSyntaxTree &t, FormatResolve &resolve) {
    auto it = _indent.find(n.GetIndex());
    if (it != _indent.end()) {
        auto &indentData = it->second;
        switch (indentData.Strategy) {
            case IndentStrategy::Standard: {
                resolve.SetIndent(IndentStrategy::Standard);
                break;
            }
            case IndentStrategy::WhenLineBreak: {
                if (f.ShouldMeetIndent()) {
                    indentData.Strategy = IndentStrategy::Standard;
                    resolve.SetIndent(IndentStrategy::Standard);
                }
                break;
            }
            case IndentStrategy::WhenPrevIndent: {
                auto prev = n.GetPrevSibling(t);
                if (_indentMark.contains(prev.GetIndex())) {
                    resolve.SetIndent(IndentStrategy::Standard);
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
