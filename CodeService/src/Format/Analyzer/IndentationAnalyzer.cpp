#include "CodeService/Format/Analyzer/IndentationAnalyzer.h"
#include "CodeService/Format/FormatBuilder.h"

// 但是我不能这样做
//using enum LuaSyntaxNodeKind;
using NodeKind = LuaSyntaxNodeKind;
using MultiKind = LuaSyntaxMultiKind;

IndentationAnalyzer::IndentationAnalyzer()
        : _style(IndentStyle::Space) {
}

void IndentationAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    if (syntaxNode.GetIndex() == 1) {
//                        Indenter(syntaxNode, IndentStyle::Space, 0);
                    } else {
                        Indenter(syntaxNode);
                    }

                    break;
                }
                case LuaSyntaxNodeKind::ParamList: {
                    Indenter(syntaxNode);
                    break;
                }
                case LuaSyntaxNodeKind::CallExpression: {
                    if (syntaxNode.GetChildToken('(', t).IsToken(t)) {
                        auto exprList = syntaxNode.GetChildSyntaxNode(NodeKind::ExpressionList, t);
                        if (exprList.IsNode(t)) {
                            Indenter(exprList);
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
                        Indenter(expr);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::IfStatement: {
                    auto exprs = syntaxNode.GetChildSyntaxNodes(MultiKind::Expression, t);
                    for (auto expr: exprs) {
                        Indenter(expr);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ExpressionStatement: {
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void IndentationAnalyzer::Indenter(LuaSyntaxNode &n) {
//    _indentMark.insert(n);
}

std::optional<IndentState> IndentationAnalyzer::GetIndentState(LuaSyntaxNode &n) const {
//    auto it = _indentMark.find(n.GetIndex());
//    if (it != _indentMark.end()) {
//        return it->second;
//    }
    return {};
}

void IndentationAnalyzer::AnalyzeExprList(FormatBuilder &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t) {
    auto exprs = exprList.GetChildSyntaxNodes(MultiKind::Expression, t);
    if (exprs.size() == 1) {
        auto expr = exprs.front();
        auto syntaxKind = expr.GetSyntaxKind(t);
        if (syntaxKind == LuaSyntaxNodeKind::ClosureExpression
            || syntaxKind == LuaSyntaxNodeKind::TableExpression) {
            return;
        }
    }
    Indenter(exprList);
}

void IndentationAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &nodes, const LuaSyntaxTree &t, FormatResolve& resolve) {
//    for (auto syntaxNode: nodes) {
//        if (!syntaxNode.GetFirstChild(t).IsNull(t)) {
//            auto indentState = GetIndentState(syntaxNode);
//            if (indentState.has_value()) {
//                AddIndent(indentState);
//                auto children = syntaxNode.GetChildren(t);
//                for (auto child: children) {
//                    ProcessIndent(child);
//                }
//            }
//        }
//    }
}


