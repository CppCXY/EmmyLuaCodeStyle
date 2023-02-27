#include "CodeService/Format/Analyzer/SpaceAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatState.h"
#include "CodeService/Config/LanguageTranslator.h"
#include "CodeService/Format/Analyzer/TokenAnalyzer.h"


SpaceAnalyzer::SpaceAnalyzer() {

}

void SpaceAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsToken(t)) {
            switch (syntaxNode.GetTokenKind(t)) {
                // math operator
                case '+':
                case '*':
                case '/':
                case '%':
                case '&':
                case '^':
                case TK_SHL:
                case TK_SHR:
                case TK_IDIV: {
                    SpaceAround(syntaxNode, t, f.GetStyle().space_around_math_operator ? 1 : 0);
                    break;
                }
                case TK_CONCAT: {
                    SpaceAround(syntaxNode, t, f.GetStyle().space_around_concat_operator ? 1 : 0);
                    break;
                }
                case '=':
                case TK_AND:
                case TK_OR:
                case TK_GE:
                case TK_LE:
                case TK_NE:
                case TK_EQ:
                case TK_IN: {
                    SpaceAround(syntaxNode, t, 1);
                    break;
                }
                case TK_NOT: {
                    SpaceRight(syntaxNode, t);
                    break;
                }
                case '-':
                case '~': {
                    auto p = syntaxNode.GetParent(t);
                    if (p.IsNode(t) && p.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
                        SpaceAround(syntaxNode, t, f.GetStyle().space_around_math_operator ? 1 : 0);
                    } else {
                        SpaceRight(syntaxNode, t, 0);
                        auto rightSiblingKind = syntaxNode.GetNextSibling(t).GetSyntaxKind(t);
                        SpaceRight(syntaxNode, t, rightSiblingKind == LuaSyntaxNodeKind::UnaryExpression ? 1 : 0);
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
                case ',': {
                    SpaceLeft(syntaxNode, t, 0);
                    SpaceRight(syntaxNode, t, f.GetStyle().space_after_comma ? 1 : 0);
                    break;
                }
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
                case TK_ELSE:
                case TK_FUNCTION:
                case TK_END: {
                    SpaceRight(syntaxNode, t);
                    break;
                }
                case TK_THEN:
                case TK_DO:
                case TK_UNTIL:
                case TK_WHILE: {
                    SpaceAround(syntaxNode, t);
                    break;
                }
                case '.':
                case ':':
                case '[':
                case ']': {
                    SpaceAround(syntaxNode, t, 0);
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
                case TK_LONG_COMMENT:
                case TK_SHORT_COMMENT: {
                    SpaceAround(syntaxNode, t, f.GetStyle().space_before_inline_comment);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

void SpaceAnalyzer::ComplexAnalyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::CallExpression: {
                    auto leftBrace = syntaxNode.GetChildToken('(', t);
                    if (leftBrace.IsToken(t)) {
                        if (f.GetStyle().space_inside_function_call_parentheses
                            && leftBrace.GetNextToken(t).GetTokenKind(t) != ')') {
                            auto rightBrace = syntaxNode.GetChildToken(')', t);
                            SpaceRight(leftBrace, t, 1);
                            SpaceLeft(rightBrace, t, 1);
                        }
                        if (f.GetStyle().ignore_spaces_inside_function_call) {
                            auto exprList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                            if (exprList.IsNode(t)) {
                                auto commas = exprList.GetChildTokens(',', t);
                                for (auto &comma: commas) {
                                    SpaceIgnore(comma, t);
                                }
                            }
                        }

                        auto leftToken = leftBrace.GetPrevToken(t);
                        if (leftToken.GetTokenKind(t) != TK_STRING
                            && leftToken.GetTokenKind(t) != '}') {
                            if (f.GetStyle().space_before_function_call_open_parenthesis) {
                                SpaceLeft(leftBrace, t, 1);
                            } else {
                                auto tokenAnalyzer = f.GetAnalyzer<TokenAnalyzer>();
                                if (!tokenAnalyzer->IsRemove(leftBrace, t)) {
                                    SpaceLeft(leftBrace, t, 0);
                                }
                            }
                        } else {
                            SpaceLeft(leftBrace, t, 1);
                        }
                    } else {
                        switch (f.GetStyle().space_before_function_call_single_arg) {
                            case FunctionSingleArgSpace::None: {
                                SpaceLeft(syntaxNode, t, 0);
                                break;
                            }
                            case FunctionSingleArgSpace::Always: {
                                SpaceLeft(syntaxNode, t, 1);
                                break;
                            }
                            case FunctionSingleArgSpace::OnlyString: {
                                auto firstToken = syntaxNode.GetFirstToken(t);
                                if (firstToken.GetTokenKind(t) == TK_STRING
                                    || firstToken.GetTokenKind(t) == TK_LONG_STRING) {
                                    SpaceLeft(syntaxNode, t, 1);
                                } else {
                                    SpaceLeft(syntaxNode, t, 0);
                                }
                                break;
                            }
                            case FunctionSingleArgSpace::OnlyTable: {
                                auto firstChild = syntaxNode.GetFirstChild(t);
                                if (firstChild.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression) {
                                    SpaceLeft(syntaxNode, t, 1);
                                } else {
                                    SpaceLeft(syntaxNode, t, 0);
                                }
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::TableExpression: {
                    auto leftCurly = syntaxNode.GetChildToken('{', t);
                    if (leftCurly.GetNextToken(t).GetTokenKind(t) != '}'
                        && f.GetStyle().space_around_table_field_list) {
                        SpaceRight(leftCurly, t, 1);
                        auto rightCurly = syntaxNode.GetChildToken('}', t);
                        SpaceLeft(rightCurly, t, 1);
                    } else {
                        SpaceRight(leftCurly, t, 0);
                        auto rightCurly = syntaxNode.GetChildToken('}', t);
                        SpaceLeft(rightCurly, t, 0);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::IndexExpression: {
                    auto leftSquareBracket = syntaxNode.GetChildToken('[', t);
                    auto rightSquareBracket = syntaxNode.GetChildToken(']', t);
                    if (leftSquareBracket.IsToken(t) && rightSquareBracket.IsToken(t)) {
                        if (f.GetStyle().space_before_open_square_bracket) {
                            SpaceLeft(leftSquareBracket, t, 1);
                        }

                        if (f.GetStyle().space_inside_square_brackets) {
                            SpaceRight(leftSquareBracket, t, 1);
                            SpaceLeft(rightSquareBracket, t, 1);
                        } else {
                            auto tokenKindAfterSquareBracket = leftSquareBracket.GetNextToken(t).GetTokenKind(t);
                            auto tokenKindBeforeSquareBracket = rightSquareBracket.GetPrevToken(t).GetTokenKind(t);
                            if (tokenKindAfterSquareBracket == TK_LONG_STRING
                                || tokenKindAfterSquareBracket == TK_LONG_COMMENT
                                || tokenKindBeforeSquareBracket == TK_LONG_COMMENT
                                || tokenKindBeforeSquareBracket == TK_LONG_STRING) {
                                SpaceRight(leftSquareBracket, t, 1);
                                SpaceLeft(rightSquareBracket, t, 1);
                            }
                        }

                        if (f.GetStyle().space_around_table_append_operator) {
                            auto binaryExpr = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::BinaryExpression, t);
                            if (binaryExpr.IsNode(t)) {
                                auto plus = binaryExpr.GetChildToken('+', t);
                                if (plus.IsToken(t)) {
                                    auto exprs = binaryExpr.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
                                    if (exprs.size() == 2) {
                                        auto leftExpr = exprs[0];
                                        auto rightExpr = exprs[1];
                                        if (leftExpr.GetSyntaxKind(t) == LuaSyntaxNodeKind::UnaryExpression
                                            && leftExpr.GetChildToken('#', t).IsToken(t)
                                            && rightExpr.GetSyntaxKind(t) == LuaSyntaxNodeKind::LiteralExpression
                                            && rightExpr.GetText(t) == "1") {
                                            SpaceAround(plus, t, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (f.GetStyle().ignore_space_after_colon) {
                        auto colon = syntaxNode.GetChildToken(':', t);
                        if (colon.IsToken(t)) {
                            SpaceIgnore(colon, t);
                        }
                    }

                    break;
                }
                case LuaSyntaxNodeKind::Attribute: {
                    if (f.GetStyle().space_before_attribute) {
                        SpaceLeft(syntaxNode, t, 1);
                    } else {
                        SpaceLeft(syntaxNode, t, 0);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::FunctionBody: {
                    auto leftBrace = syntaxNode.GetChildToken('(', t);
                    if (syntaxNode.GetParent(t).GetSyntaxKind(t) == LuaSyntaxNodeKind::ClosureExpression) {
                        if (f.GetStyle().space_before_closure_open_parenthesis) {
                            SpaceLeft(leftBrace, t, 1);
                        } else {
                            SpaceLeft(leftBrace, t, 0);
                        }
                    } else {
                        if (f.GetStyle().space_before_function_open_parenthesis) {
                            SpaceLeft(leftBrace, t, 1);
                        } else {
                            SpaceLeft(leftBrace, t, 0);
                        }
                    }
                    if (f.GetStyle().space_inside_function_param_list_parentheses) {
                        auto next = leftBrace.GetNextToken(t);
                        if (next.GetTokenKind(t) != ')') {
                            SpaceRight(leftBrace, t, 1);
                            auto rightBrace = syntaxNode.GetChildToken(')', t);
                            SpaceLeft(rightBrace, t, 1);
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ForNumber: {
                    if (!f.GetStyle().space_after_comma_in_for_statement) {
                        auto commas = syntaxNode.GetChildTokens(',', t);
                        for (auto comma: commas) {
                            SpaceRight(comma, t, 0);
                        }
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ForList: {
                    if (!f.GetStyle().space_after_comma_in_for_statement) {
                        auto nameList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::NameDefList, t);
                        for (auto comma: nameList.GetChildTokens(',', t)) {
                            SpaceRight(comma, t, 0);
                        }
                        auto exprList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                        for (auto comma: exprList.GetChildTokens(',', t)) {
                            SpaceRight(comma, t, 0);
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

void SpaceAnalyzer::Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        auto nextToken = syntaxNode.GetNextToken(t);
        auto space = ProcessSpace(t, syntaxNode, nextToken);
        resolve.SetNextSpace(space);
    }
}

void SpaceAnalyzer::SpaceAround(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space) {
    SpaceLeft(n, t, space);
    SpaceRight(n, t, space);
}

void SpaceAnalyzer::SpaceLeft(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space) {
    auto token = n.GetPrevToken(t);
    _rightSpaces[token.GetIndex()] = space;
}

void SpaceAnalyzer::SpaceRight(LuaSyntaxNode &n, const LuaSyntaxTree &t, std::size_t space) {
    auto token = n.GetLastToken(t);
    _rightSpaces[token.GetIndex()] = space;
}

SpaceAnalyzer::OptionalInt SpaceAnalyzer::GetRightSpace(LuaSyntaxNode &n) const {
    if (_ignoreSpace.count(n.GetIndex())) {
        return OptionalInt();
    }

    auto it = _rightSpaces.find(n.GetIndex());
    if (it == _rightSpaces.end()) {
        return OptionalInt();
    }
    return OptionalInt(it->second);
}

std::size_t
SpaceAnalyzer::ProcessSpace(const LuaSyntaxTree &t, LuaSyntaxNode &left, LuaSyntaxNode &right) {
    auto rightSpaceOfLeftToken = GetRightSpace(left);
    if (rightSpaceOfLeftToken.HasValue) {
        return rightSpaceOfLeftToken.Value;
    }
    if (!right.IsNull(t)) {
        return t.GetStartOffset(right.GetIndex()) - t.GetEndOffset(left.GetIndex()) - 1;
    }
    return 0;
}

void SpaceAnalyzer::SpaceIgnore(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    _ignoreSpace.insert(n.GetIndex());
}
