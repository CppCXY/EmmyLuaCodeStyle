#include "CodeService/Format/Analyzer/LineBreakAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

using NodeKind = LuaSyntaxNodeKind;

LineBreakAnalyzer::LineBreakAnalyzer() {

}

void LineBreakAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
//    for (auto syntaxNode: t.GetSyntaxNodes()) {
//        if (syntaxNode.IsNode(t)) {
//            switch (syntaxNode.GetSyntaxKind(t)) {
//                case LuaSyntaxNodeKind::Block: {
//                    if (syntaxNode.GetParent(t).IsSingleLineNode(t)) {
//                        if (syntaxNode.GetChildren(t).size() > 1) {
//                            BreakTo(syntaxNode, LineBreakStrategy::MultiLine);
//                            continue;
//                        }
//                    }
//                    for (auto stmt: syntaxNode.GetChildren(t)) {
//                        BreakAfter(syntaxNode);
//                    }
//                    break;
//                }
//                case LuaSyntaxNodeKind::LocalStatement:
//                case LuaSyntaxNodeKind::AssignStatement:
//                case LuaSyntaxNodeKind::ReturnStatement: {
//                    auto exprList = syntaxNode.GetChildSyntaxNode(NodeKind::ExpressionList, t);
//                    if (exprList.IsNode(t)) {
//                        AnalyzeExprList(f, exprList, t);
//                    }
//                    break;
//                }
//                case LuaSyntaxNodeKind::IfStatement:
//                case LuaSyntaxNodeKind::WhileStatement:
//                case LuaSyntaxNodeKind::RepeatStatement: {
//                    auto expr = syntaxNode.GetChildSyntaxNode(LuaSyntaxMultiKind::Expression, t);
//                    if (expr.IsNode(t)) {
//                        AnalyzeConditionExpr(f, expr, t);
//                    }
//                    break;
//                }
//                case LuaSyntaxNodeKind::ForStatement: {
//                    auto nameList = syntaxNode.GetChildSyntaxNode(NodeKind::NameDefList, t);
//                    if(nameList.IsNode(t)) {
//                        AnalyzeNameList(f, nameList, t);
//                    }
//                    auto exprList = syntaxNode.GetChildSyntaxNode(NodeKind::ExpressionList, t);
//                    if (exprList.IsNode(t)) {
//                        AnalyzeExprList(f, exprList, t);
//                    }
//                    break;
//                }
//                case LuaSyntaxNodeKind::FunctionBody: {
//                    auto paramList = syntaxNode.GetChildSyntaxNode(NodeKind::ParamList, t);
//                    if(paramList.IsNode(t)){
//                        AnalyzeNameList(f, paramList, t);
//                    }
//                    break;
//                }
//                case LuaSyntaxNodeKind::CallExpression: {
//
//                    break;
//                }
//            }
//        } else {
//            switch (syntaxNode.GetTokenKind(t)) {
//                case TK_SHORT_COMMENT:
//                case TK_LONG_COMMENT:
//                case TK_SHEBANG: {
//                    BreakAfter(syntaxNode);
//                }
//            }
//        }
//    }
}

void
LineBreakAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {

}
