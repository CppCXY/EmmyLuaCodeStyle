#include "SpaceAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatBuilder.h"

SpaceAnalyzer::SpaceAnalyzer() {

}

void SpaceAnalyzer::Analyze(FormatBuilder &f, std::vector<LuaSyntaxNode> &nodes, const LuaSyntaxTree &t) {
    for (auto &syntaxNode: nodes) {
        if (syntaxNode.IsToken(t)) {
            switch (syntaxNode.GetTokenKind(t)) {
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
                    f.SpaceAround(syntaxNode);
                    break;
                }
                case TK_NOT: {
                    f.SpaceRight(syntaxNode);
                    break;
                }
                case '(': {
                    f.SpaceRight(syntaxNode, 0);
                    break;
                }
                case '-':
                case '~': {
                    auto p = syntaxNode.GetParent(t);
                    if (p.IsNode(t) && p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        f.SpaceAround(syntaxNode);
                    } else {
                        f.SpaceRight(syntaxNode);
                    }
                    break;
                }
                case ')': {
                    f.SpaceLeft(syntaxNode, 0);
                    break;
                }
                case '<':
                case '>': {
                    auto p = syntaxNode.GetParent(t);
                    if (p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        f.SpaceAround(syntaxNode);
                    } else if (syntaxNode.GetTokenKind(t) == '<') {
                        f.SpaceRight(syntaxNode, 0);
                    } else {
                        f.SpaceLeft(syntaxNode, 0);
                    }
                    break;
                }
                case ',':
                case ';': {
                    f.SpaceLeft(syntaxNode, 0);
                    f.SpaceRight(syntaxNode, 1);
                }
                default: {
                    break;
                }
            }
        }
    }
}
