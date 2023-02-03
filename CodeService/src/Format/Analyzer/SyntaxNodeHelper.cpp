#include <queue>
#include "SyntaxNodeHelper.h"

std::vector<LuaSyntaxNode> helper::CollectBinaryOperator(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t,
                                                         const std::function<bool(LuaTokenKind)> &predicated) {
    std::vector<LuaSyntaxNode> results;

    std::queue<LuaSyntaxNode> nodeQueue;
    nodeQueue.push(syntaxNode);
    do {
        auto n = nodeQueue.front();
        nodeQueue.pop();
        if (n.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
            auto op = n.GetChildToken(predicated, t);
            if (op.IsToken(t)) {
                results.push_back(op);
            }
            auto subBinaryExprs = n.GetChildSyntaxNodes(LuaSyntaxNodeKind::BinaryExpression, t);
            for (auto b: subBinaryExprs) {
                nodeQueue.push(b);
            }
        }
    } while (!nodeQueue.empty());
    return results;
}
