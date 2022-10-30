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
//                    f.NewLineIntenter(syntaxNode);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

