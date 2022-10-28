#include "IntentionAnalyzer.h"

void IntentionAnalyzer::Analyze(FormatBuilder &f, std::vector<LuaAstNode> &nodes, const LuaAstTree &t) {
    for (auto &astNode: nodes) {
        if (astNode.IsNode(t)) {
            switch (astNode.GetType(t)) {
                case LuaNodeType::Block: {
                    f.Intenter(astNode);
                    break;
                }
                case LuaNodeType::LocalStatement:
                case LuaNodeType::AssignStatement: {
                    f.NewLineIntenter(astNode);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}
