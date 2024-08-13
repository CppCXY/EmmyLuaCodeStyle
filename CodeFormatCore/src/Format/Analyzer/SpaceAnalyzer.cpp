#include "CodeFormatCore/Format/Analyzer/SpaceAnalyzer.h"
#include "CodeFormatCore/Config/LanguageTranslator.h"
#include "CodeFormatCore/Format/Analyzer/TokenAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"


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
                case TK_SHL:
                case TK_SHR:
                case TK_IDIV: {
                    SpaceAround(syntaxNode, t, f.GetStyle().space_around_math_operator.other ? 1 : 0);
                    break;
                }
                case '^': {
                    SpaceAround(syntaxNode, t, f.GetStyle().space_around_math_operator.exponent ? 1 : 0);
                    break;
                }
                case TK_CONCAT: {
                    switch (f.GetStyle().space_around_concat_operator) {
                        case SpaceAroundStyle::Always: {
                            SpaceAround(syntaxNode, t, 1);
                            break;
                        }
                        case SpaceAroundStyle::None: {
                            if (syntaxNode.GetPrevToken(t).GetTokenKind(t) == TK_NUMBER) {
                                SpaceAround(syntaxNode, t, 1);
                            } else {
                                SpaceAround(syntaxNode, t, 0);
                            }
                            break;
                        }
                        case SpaceAroundStyle::NoSpaceAsym: {
                            if (syntaxNode.GetPrevToken(t).GetTokenKind(t) == TK_NUMBER) {
                                SpaceLeft(syntaxNode, t, 1);
                                SpaceRight(syntaxNode, t, 0);
                            } else {
                                SpaceAround(syntaxNode, t, 0);
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }
                case '=': {
                    switch (f.GetStyle().space_around_assign_operator) {
                        case SpaceAroundStyle::Always: {
                            SpaceAround(syntaxNode, t, 1);
                            break;
                        }
                        case SpaceAroundStyle::None: {
                            if (syntaxNode.GetPrevToken(t).GetTokenKind(t) == '>') {
                                SpaceAround(syntaxNode, t, 1);
                            } else {
                                SpaceAround(syntaxNode, t, 0);
                            }
                            break;
                        }
                        case SpaceAroundStyle::NoSpaceAsym: {
                            if (syntaxNode.GetPrevToken(t).GetTokenKind(t) == '>') {
                                SpaceLeft(syntaxNode, t, 1);
                                SpaceRight(syntaxNode, t, 0);
                            } else {
                                SpaceAround(syntaxNode, t, 0);
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }
                case TK_GE:
                case TK_LE:
                case TK_NE:
                case TK_EQ: {
                    SpaceAround(syntaxNode, t, f.GetStyle().space_around_logical_operator ? 1 : 0);
                    break;
                }
                case TK_AND:
                case TK_OR:
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
                        SpaceAround(syntaxNode, t, f.GetStyle().space_around_math_operator.other ? 1 : 0);
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
                        SpaceAround(syntaxNode, t, f.GetStyle().space_around_logical_operator ? 1 : 0);
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
                case ']': {
                    SpaceAround(syntaxNode, t, 0);
                }
                case '[': {
                    auto prevKind = syntaxNode.GetPrevToken(t).GetTokenKind(t);
                    if (prevKind == ',' || prevKind == ';') {
                        SpaceRight(syntaxNode, t, 0);
                    } else {
                        SpaceAround(syntaxNode, t, 0);
                    }
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
                    if (f.GetStyle().space_before_inline_comment.Style == SpaceBeforeInlineCommentStyle::Fixed) {
                        SpaceLeft(syntaxNode, t, f.GetStyle().space_before_inline_comment.Space, SpacePriority::First);
                    } else {
                        auto prevToken = syntaxNode.GetPrevToken(t);
                        if (prevToken.GetEndLine(t) == syntaxNode.GetStartLine(t)) {
                            auto space = syntaxNode.GetStartCol(t) - prevToken.GetEndCol(t) - 1;
                            SpaceLeft(syntaxNode, t, space, SpacePriority::First);
                        } else {
                            SpaceLeft(syntaxNode, t, 0, SpacePriority::First);
                        }
                    }
                    SpaceRight(syntaxNode, t, 1);
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
                        if (f.GetStyle().space_inside_function_call_parentheses &&
                            leftBrace.GetNextToken(t).GetTokenKind(t) != ')') {
                            auto rightBrace = syntaxNode.GetChildToken(')', t);
                            SpaceRight(leftBrace, t, 1);
                            SpaceLeft(rightBrace, t, 1);
                        }
                        if (f.GetStyle().ignore_spaces_inside_function_call) {
                            auto exprList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                            if (exprList.IsNode(t)) {
                                auto commas = exprList.GetChildTokens(',', t);
                                for (auto &comma: commas) {
                                    SpaceIgnore(comma);
                                }
                            }
                        }

                        auto leftToken = leftBrace.GetPrevToken(t);
                        if (leftToken.GetTokenKind(t) != TK_STRING && leftToken.GetTokenKind(t) != '}') {
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
                        FunctionCallSingleArgSpace(f, syntaxNode, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::TableExpression: {
                    auto leftCurly = syntaxNode.GetChildToken('{', t);
                    if (leftCurly.GetNextToken(t).GetTokenKind(t) != '}' &&
                        f.GetStyle().space_around_table_field_list) {
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
                            if (tokenKindAfterSquareBracket == TK_LONG_STRING ||
                                tokenKindAfterSquareBracket == TK_LONG_COMMENT ||
                                tokenKindBeforeSquareBracket == TK_LONG_COMMENT ||
                                tokenKindBeforeSquareBracket == TK_LONG_STRING) {
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
                                        if (leftExpr.GetSyntaxKind(t) == LuaSyntaxNodeKind::UnaryExpression &&
                                            leftExpr.GetChildToken('#', t).IsToken(t) &&
                                            rightExpr.GetSyntaxKind(t) == LuaSyntaxNodeKind::LiteralExpression &&
                                            rightExpr.GetText(t) == "1") {
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
                            SpaceIgnore(colon);
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

void SpaceAnalyzer::Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        auto nextToken = syntaxNode.GetNextToken(t);
        auto space = ProcessSpace(syntaxNode, nextToken, t);
        resolve.SetNextSpace(space);
    }
}

void SpaceAnalyzer::SpaceAround(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t space, SpacePriority priority) {
    SpaceLeft(n, t, space, priority);
    SpaceRight(n, t, space, priority);
}

void SpaceAnalyzer::SpaceLeft(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t space, SpacePriority priority) {
    auto token = n.GetPrevToken(t);
    auto it = _rightSpaces.find(token.GetIndex());
    if (it != _rightSpaces.end()) {
        if (it->second.Priority > priority) {
            return;
        }
    }

    _rightSpaces[token.GetIndex()] = SpaceData(space, priority);
}

void SpaceAnalyzer::SpaceRight(LuaSyntaxNode n, const LuaSyntaxTree &t, std::size_t space, SpacePriority priority) {
    auto token = n.GetLastToken(t);
    auto it = _rightSpaces.find(token.GetIndex());
    if (it != _rightSpaces.end()) {
        if (it->second.Priority > priority) {
            return;
        }
    }

    _rightSpaces[token.GetIndex()] = SpaceData(space, priority);
}

void SpaceAnalyzer::SpaceIgnore(LuaSyntaxNode n) {
    _ignoreSpace.insert(n.GetIndex());
}

void SpaceAnalyzer::FunctionCallSingleArgSpace(FormatState &f, LuaSyntaxNode n, const LuaSyntaxTree &t) {
    auto firstToken = n.GetFirstToken(t);
    if (firstToken.GetTokenKind(t) == TK_STRING ||
        firstToken.GetTokenKind(t) == TK_LONG_STRING) {
        switch (f.GetStyle().space_before_function_call_single_arg.string) {
            case FunctionSingleArgSpace::None: {
                SpaceLeft(firstToken, t, 0);
                break;
            }
            case FunctionSingleArgSpace::Always: {
                SpaceLeft(firstToken, t, 1);
                break;
            }
            case FunctionSingleArgSpace::Keep: {
                SpaceIgnore(firstToken);
                break;
            }
            default: {
                break;
            }
        }
    } else {
        switch (f.GetStyle().space_before_function_call_single_arg.table) {
            case FunctionSingleArgSpace::None: {
                SpaceLeft(firstToken, t, 0);
                break;
            }
            case FunctionSingleArgSpace::Always: {
                SpaceLeft(firstToken, t, 1);
                break;
            }
            case FunctionSingleArgSpace::Keep: {
                SpaceIgnore(firstToken);
                break;
            }
            default: {
                break;
            }
        }
    }
}

std::size_t SpaceAnalyzer::GetRightSpace(LuaSyntaxNode n, bool &hasValue) const {
    if (_ignoreSpace.count(n.GetIndex())) {
        hasValue = false;
        return 0;
    }

    auto it = _rightSpaces.find(n.GetIndex());
    if (it == _rightSpaces.end()) {
        hasValue = false;
        return 0;
    }
    hasValue = true;
    return it->second.Value;
}

std::size_t
SpaceAnalyzer::ProcessSpace(LuaSyntaxNode left, LuaSyntaxNode right, const LuaSyntaxTree &t) {
    bool hasValue = true;
    auto rightSpaceOfLeftToken = GetRightSpace(left, hasValue);
    if (hasValue) {
        return rightSpaceOfLeftToken;
    }
    if (!right.IsNull(t)) {
        return t.GetStartOffset(right.GetIndex()) - t.GetEndOffset(left.GetIndex()) - 1;
    }
    return 0;
}
