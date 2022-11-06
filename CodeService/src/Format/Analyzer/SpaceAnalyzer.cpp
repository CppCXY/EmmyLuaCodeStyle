#include "CodeService/Format/Analyzer/SpaceAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatBuilder.h"


SpaceAnalyzer::SpaceAnalyzer() {

}

void SpaceAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
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
                case TK_NE:
                case TK_EQ:
                case TK_IDIV:
                case TK_CONCAT:
                case TK_IN: {
                    SpaceAround(syntaxNode);
                    break;
                }
                case TK_NOT: {
                    SpaceRight(syntaxNode);
                    break;
                }
                case '(': {
                    SpaceRight(syntaxNode, 0);
                    break;
                }
                case '-':
                case '~': {
                    auto p = syntaxNode.GetParent(t);
                    if (p.IsNode(t) && p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        SpaceAround(syntaxNode);
                    } else {
                        SpaceRight(syntaxNode);
                    }
                    break;
                }
                case ')': {
                    SpaceLeft(syntaxNode, 0);
                    break;
                }
                case '<':
                case '>': {
                    auto p = syntaxNode.GetParent(t);
                    if (p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        SpaceAround(syntaxNode);
                    } else if (syntaxNode.GetTokenKind(t) == '<') {
                        SpaceRight(syntaxNode, 0);
                    } else {
                        SpaceLeft(syntaxNode, 0);
                    }
                    break;
                }
                case ',':
                case ';': {
                    SpaceLeft(syntaxNode, 0);
                    SpaceRight(syntaxNode, 1);
                    break;
                }
                case TK_IF:
                case TK_LOCAL:
                case TK_ELSEIF:
                case TK_RETURN:
                case TK_GOTO:
                case TK_FOR: {
                    SpaceRight(syntaxNode);
                    break;
                }
                case TK_THEN: {
                    SpaceLeft(syntaxNode);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void SpaceAnalyzer::Process(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
//    std::vector<LuaSyntaxNode> tokens;
//    for (auto syntaxNode: nodes) {
//        if (syntaxNode.IsToken(t)) {
//            tokens.push_back(syntaxNode);
//        }
//    }
//
//    for (std::size_t i = 1; i != tokens.size(); i++) {
//        auto token = tokens[i];
//        if (i == 1) {
//            auto prevToken = tokens[i - 1];
//            ProcessSpace(f, t, prevToken, token);
//        }
//        if (i + 1 < tokens.size()) {
//            auto nextToken = tokens[i + 1];
//            ProcessSpace(f, t, token, nextToken);
//        }
//    }

}


void SpaceAnalyzer::SpaceAround(LuaSyntaxNode &n, std::size_t space) {
    SpaceLeft(n, space);
    SpaceRight(n, space);
}

void SpaceAnalyzer::SpaceLeft(LuaSyntaxNode &n, std::size_t space) {
    _leftSpaces[n.GetIndex()] = space;
}

void SpaceAnalyzer::SpaceRight(LuaSyntaxNode &n, std::size_t space) {
    _rightSpaces[n.GetIndex()] = space;
}

std::optional<std::size_t> SpaceAnalyzer::GetLeftSpace(LuaSyntaxNode &n) const {
    auto it = _leftSpaces.find(n.GetIndex());
    if (it != _leftSpaces.end()) {
        return it->second;
    }
    return {};
}

std::optional<std::size_t> SpaceAnalyzer::GetRightSpace(LuaSyntaxNode &n) const {
    auto it = _rightSpaces.find(n.GetIndex());
    if (it != _rightSpaces.end()) {
        return it->second;
    }
    return {};
}

void SpaceAnalyzer::ProcessSpace(FormatBuilder &f, const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right) {
    auto rightSpaceOfLeftToken = GetRightSpace(left);
    auto leftSpaceOfRightToken = GetLeftSpace(right);
    std::optional<std::size_t> distance;
    if (!rightSpaceOfLeftToken.has_value()) {
        distance = leftSpaceOfRightToken;
    } else if (!leftSpaceOfRightToken.has_value()) {
        distance = rightSpaceOfLeftToken;
    } else {
        distance = leftSpaceOfRightToken;
    }

    if (distance.has_value() && left.GetEndLine(t) == right.GetStartLine(t)) {
        auto distanceValue = distance.value();
        auto leftEndOffset = t.GetEndOffset(left.GetIndex());
        auto rightStartOffset = t.GetStartOffset(right.GetIndex());
        if (rightStartOffset - leftEndOffset != distanceValue + 1) {
            FormatChange change;
            change.Start = leftEndOffset + 1;
            change.Length = rightStartOffset - leftEndOffset - 1;
            change.NewString.resize(distanceValue, ' ');
            f.PushChange(std::move(change));
        }
    }
}
