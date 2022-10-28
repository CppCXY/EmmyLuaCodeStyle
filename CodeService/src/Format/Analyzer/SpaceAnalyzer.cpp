#include "SpaceAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

SpaceAnalyzer::SpaceAnalyzer() {

}

void SpaceAnalyzer::Analyze(FormatBuilder &f, std::vector<LuaAstNode> &nodes, const LuaAstTree &t) {
    for (auto &astNode: nodes) {
        if (astNode.IsToken(t)) {
            switch (astNode.GetTokenType(t)) {
                case '+':
                case '*':
                case '/':
                case '%':
                case TK_AND:
                case TK_OR:
                case '&':
                case '^':
                case TK_SHL:
                case TK_SHR:
                case TK_GE:
                case TK_LE:
                case TK_NE: {
                    f.SpaceAround(astNode);
                    break;
                }
                case TK_NOT:
                case '(': {
                    f.SpaceRight(astNode);
                    break;
                }
                case '-':
                case '~': {
                    auto p = astNode.GetParent(t);
                    if (p.IsNode(t) && p.GetType(t) == LuaNodeType::BinaryExpression) {
                        f.SpaceAround(astNode);
                    } else {
                        f.SpaceRight(astNode);
                    }
                    break;
                }
                case ')': {
                    f.SpaceLeft(astNode);
                    break;
                }
                case '<':
                case '>': {
                    auto p = astNode.GetParent(t);
                    if (p.GetType(t) == LuaNodeType::BinaryExpression) {
                        f.SpaceAround(astNode);
                    } else if (astNode.GetTokenType(t) == '<') {
                        f.SpaceRight(astNode, 0);
                    } else {
                        f.SpaceLeft(astNode, 0);
                    }
                    break;
                }
                case ',':
                case ';': {
                    f.SpaceLeft(astNode, 0);
                    f.SpaceRight(astNode, 1);
                }
                default: {
                    break;
                }
            }
        }
    }
}

