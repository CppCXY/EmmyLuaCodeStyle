#include "SpaceAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatBuilder.h"

SpaceAnalyzer::SpaceAnalyzer() {

}

void SpaceAnalyzer::Analyze(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) {
    for (auto &astNode: nodes) {
        if (astNode.IsToken(t)) {
            switch (astNode.GetTokenKind(t)) {
                case '+':
                case '*':
                case '/':
                case '%':
                case '=':
                case '&':
                case '^':
                case TK_AND:
                case TK_OR:
                case TK_SHL:
                case TK_SHR:
                case TK_GE:
                case TK_LE:
                case TK_NE: {
                    f.SpaceAround(astNode);
                    break;
                }
                case TK_NOT: {
                    f.SpaceRight(astNode);
                    break;
                }
                case '(': {
                    f.SpaceRight(astNode, 0);
                    break;
                }
                case '-':
                case '~': {
                    auto p = astNode.GetParent(t);
                    if (p.IsNode(t) && p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        f.SpaceAround(astNode);
                    } else {
                        f.SpaceRight(astNode);
                    }
                    break;
                }
                case ')': {
                    f.SpaceLeft(astNode, 0);
                    break;
                }
                case '<':
                case '>': {
                    auto p = astNode.GetParent(t);
                    if (p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        f.SpaceAround(astNode);
                    } else if (astNode.GetTokenKind(t) == '<') {
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
