#include "CodeService/Format/Analyzer/LineBreakAnalyzer.h"
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
        }
    }
}

void
LineBreakAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {

    if (syntaxNode.IsToken(t)) {
        auto it = _rightLines.find(syntaxNode.GetIndex());
        if (it != _rightLines.end()) {
            resolve.SetNextLineBreak(it->second);
            return;
        }
        if (resolve.GetSpaceStrategy() == SpaceStrategy::None
            || resolve.GetSpaceStrategy() == SpaceStrategy::Space) {
            auto nextToken = syntaxNode.GetNextToken(t);
            if (nextToken.IsToken(t)) {
                auto currentLine = syntaxNode.GetEndLine(t);
                auto nextLine = nextToken.GetStartLine(t);
                if (nextLine > currentLine) {
                    resolve.SetNextLineBreak(nextLine - currentLine);
                }
            }
        }
    }
}

void LineBreakAnalyzer::BreakAfter(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t line) {
    auto token = n.GetLastToken(t);
    if (token.IsToken(t)) {
        _rightLines[token.GetIndex()] = line;
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
            MarkLazyBreak(expr, t, LazyLineBreakStrategy::BreakWhenMayExceed);
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
        MarkLazyBreak(name, t, LazyLineBreakStrategy::BreakWhenMayExceed);
    }
}

void LineBreakAnalyzer::AnalyzeSuffixedExpr(FormatBuilder &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t) {

}

void LineBreakAnalyzer::MarkLazyBreak(LuaSyntaxNode n, const LuaSyntaxTree &t, LazyLineBreakStrategy strategy) {

}

void LineBreakAnalyzer::AnalyzeExpr(FormatBuilder &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t) {

}
