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
                    SpaceAround(syntaxNode, t);
                    break;
                }
                case TK_NOT: {
                    SpaceRight(syntaxNode, t);
                    break;
                }
                case '(': {
                    SpaceRight(syntaxNode, t, 0);
                    break;
                }
                case ')': {
                    SpaceLeft(syntaxNode, t, 0);
                    break;
                }
                case '-':
                case '~': {
                    auto p = syntaxNode.GetParent(t);
                    if (p.IsNode(t) && p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        SpaceAround(syntaxNode, t);
                    } else {
                        SpaceLeft(syntaxNode, t);
                    }
                    break;
                }
                case '<':
                case '>': {
                    auto p = syntaxNode.GetParent(t);
                    if (p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        SpaceAround(syntaxNode, t);
                    } else if (syntaxNode.GetTokenKind(t) == '<') {
                        SpaceRight(syntaxNode, t, 0);
                    } else {
                        SpaceLeft(syntaxNode, t, 0);
                    }
                    break;
                }
                case ',':
                case ';': {
                    SpaceLeft(syntaxNode, t, 0);
                    SpaceRight(syntaxNode, t, 1);
                    break;
                }
                case TK_IF:
                case TK_LOCAL:
                case TK_ELSEIF:
                case TK_RETURN:
                case TK_GOTO:
                case TK_FOR:
                case TK_FUNCTION: {
                    SpaceRight(syntaxNode, t);
                    break;
                }
                case TK_THEN: {
                    SpaceLeft(syntaxNode, t);
                    break;
                }
                case '.':
                case ':': {
                    SpaceAround(syntaxNode, t, 0);
                    break;
                }
                case TK_LONG_COMMENT: {
                    SpaceLeft(syntaxNode, t, 1);
                    break;
                }
                case TK_SHORT_COMMENT: {
                    SpaceLeft(syntaxNode, t, 1);
                    break;
                }
                default: {
                    break;
                }
            }
        } else {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::CallExpression: {
                    if (syntaxNode.GetChildToken('(', t).IsToken(t)) {
                        SpaceLeft(syntaxNode, t, 0);
                    } else {
                        SpaceLeft(syntaxNode, t, 1);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::TableExpression: {
                    auto leftCurly = syntaxNode.GetChildToken('{', t);
                    if (leftCurly.GetNextToken(t).GetTokenKind(t) == '}') {
                        SpaceRight(leftCurly, t, 0);
                        auto rightCurly = syntaxNode.GetChildToken('}', t);
                        SpaceLeft(rightCurly, t, 0);
                    } else {
                        SpaceRight(leftCurly, t, 1);
                        auto rightCurly = syntaxNode.GetChildToken('}', t);
                        SpaceLeft(rightCurly, t, 1);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::IndexExpression: {
                    auto leftSquareBracket = syntaxNode.GetChildToken('[', t);
                    auto rightSquareBracket = syntaxNode.GetChildToken(']', t);
                    if (leftSquareBracket.IsToken(t) && rightSquareBracket.IsToken(t)) {
                        auto tokenKindAfterSquareBracket = leftSquareBracket.GetNextToken(t).GetTokenKind(t);
                        auto tokenKindBeforeSquareBracket = rightSquareBracket.GetPrevToken(t).GetTokenKind(t);
                        if (tokenKindAfterSquareBracket == TK_LONG_STRING
                            || tokenKindAfterSquareBracket == TK_LONG_COMMENT
                            || tokenKindBeforeSquareBracket == TK_LONG_COMMENT
                            || tokenKindBeforeSquareBracket == TK_LONG_STRING) {
                            SpaceLeft(leftSquareBracket, t, 0);
                            SpaceRight(leftSquareBracket, t, 1);

                            SpaceLeft(rightSquareBracket, t, 1);
                            SpaceRight(rightSquareBracket, t, 0);
                        } else {
                            SpaceAround(leftSquareBracket, t, 0);
                            SpaceAround(rightSquareBracket, t, 0);
                        }
                    }

                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void SpaceAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        auto nextToken = syntaxNode.GetNextToken(t);
        auto space = ProcessSpace(f, t, syntaxNode, nextToken);
        resolve.SetNextSpace(space);
    }
}


void SpaceAnalyzer::SpaceAround(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space) {
    SpaceLeft(n, t, space);
    SpaceRight(n, t, space);
}

void SpaceAnalyzer::SpaceLeft(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space) {
    auto token = n.GetFirstToken(t);
    _leftSpaces[token.GetIndex()] = space;
}

void SpaceAnalyzer::SpaceRight(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space) {
    auto token = n.GetLastToken(t);
    _rightSpaces[token.GetIndex()] = space;
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

std::size_t
SpaceAnalyzer::ProcessSpace(FormatBuilder &f, const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right) {
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
    if (distance.has_value()) {
        return distance.value();
    }
    if (!right.IsNull(t)) {
        return t.GetStartOffset(right.GetIndex()) - t.GetEndOffset(left.GetIndex()) - 1;
    }
    return 0;
}

