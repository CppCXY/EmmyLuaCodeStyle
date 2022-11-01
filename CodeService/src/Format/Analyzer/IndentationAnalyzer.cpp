#include "IndentationAnalyzer.h"
#include "CodeService/Format/FormatBuilder.h"

// 但是我不能这样做
//using enum LuaSyntaxNodeKind;
using NodeKind = LuaSyntaxNodeKind;
using MultiKind = LuaSyntaxMultiKind;

IndentationAnalyzer::IndentationAnalyzer() {
}

void IndentationAnalyzer::Analyze(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) {
    for (auto &syntaxNode: nodes) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block:
                case LuaSyntaxNodeKind::ParamList: {
                    f.Indenter(syntaxNode);
                    break;
                }
                case LuaSyntaxNodeKind::CallExpression: {
                    if (syntaxNode.GetChildToken('(', t).IsToken(t)) {
                        auto exprList = syntaxNode.GetChildSyntaxNode(NodeKind::ExpressionList, t);
                        if (exprList.IsNode(t)) {
                            f.Indenter(exprList);
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
                        f.Indenter(expr);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::IfStatement: {
                    auto exprs = syntaxNode.GetChildSyntaxNodes(MultiKind::Expression, t);
                    for (auto expr: exprs) {
                        f.Indenter(expr);
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
    f.Indenter(exprList);
}

void IndentationAnalyzer::Process(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) {

}


