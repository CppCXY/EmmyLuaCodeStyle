#include "IndentationAnalyzer.h"
#include "CodeService/Format/FormatBuilder.h"

IndentationAnalyzer::IndentationAnalyzer() {
}

void IndentationAnalyzer::Analyze(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) {
    for (auto &syntaxNode: nodes) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    f.Indenter(syntaxNode);
                    break;
                }
                case LuaSyntaxNodeKind::LocalStatement:
                case LuaSyntaxNodeKind::AssignStatement: {
                    auto exprList = syntaxNode.ChildNodeBy()
                }
                case LuaSyntaxNodeKind::BreakStatement:
                case LuaSyntaxNodeKind::DoStatement:
                case LuaSyntaxNodeKind::WhileStatement:
                case LuaSyntaxNodeKind::IfStatement:
                case LuaSyntaxNodeKind::LocalFunctionStatement:
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

