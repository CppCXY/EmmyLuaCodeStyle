#include "CodeService/Format/Analyzer/AlignAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatState.h"
#include "SyntaxNodeHelper.h"


AlignAnalyzer::AlignAnalyzer() {

}

void AlignAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    std::unordered_map<std::size_t, std::size_t> marks;
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    if (f.GetStyle().align_continuous_assign_statement_to_equal) {
                        AnalyzeContinuousLocalOrAssign(f, syntaxNode, t);
                    }
                    if (f.GetStyle().align_continuous_similar_call_args) {
                        AnalyzeContinuousSimilarCallArgs(f, syntaxNode, t);
                    }

                    break;
                }
                case LuaSyntaxNodeKind::TableFieldList: {
                    if (f.GetStyle().align_continuous_rect_table_field_to_equal) {
                        AnalyzeContinuousTableField(f, syntaxNode, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::CallExpression: {
                    if (f.GetStyle().align_call_args != AlignCallArgs::None) {
                        auto exprlist = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                        AnalyzeExpressionList(f, exprlist, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::ParamList: {
                    if (f.GetStyle().align_function_params) {
                        AnalyzeParamList(f, syntaxNode, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::IfStatement: {
                    if (f.GetStyle().align_if_branch) {
                        AnalyzeIfStatement(f, syntaxNode, t);
                    }
                }

//                case LuaSyntaxNodeKind::ExpressionStatement: {
//                    if (f.GetStyle().align_chained_expression_statement) {
//                    }
//                    break;
//                }
                default: {
                    break;
                }
            }
        }
    }
}

void AlignAnalyzer::Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (_startNodeToGroupIndex.count(syntaxNode.GetIndex())) {
        auto alignGroupIndex = _startNodeToGroupIndex[syntaxNode.GetIndex()];
        auto &alignGroup = _alignGroup[alignGroupIndex];
        if (!alignGroup.Resolve) {
            ResolveAlignGroup(f, alignGroupIndex, alignGroup, t);
            alignGroup.Resolve = true;
        }
    }

    auto it = _resolveGroupIndex.find(syntaxNode.GetIndex());
    if (it != _resolveGroupIndex.end()) {
        auto alignGroupIndex = it->second;
        auto &alignGroup = _alignGroup[alignGroupIndex];
        switch (alignGroup.Strategy) {
            case AlignStrategy::Normal:
            case AlignStrategy::AlignToEq: {
                resolve.SetRelativeIndentAlign(alignGroup.AlignPos);
                break;
            }
            case AlignStrategy::AlignToFirst: {
                resolve.SetAlign(alignGroup.AlignPos);
                break;
            }
        }
    }
}

void AlignAnalyzer::PushAlignGroup(AlignStrategy strategy, std::vector<std::size_t> &data) {
    auto pos = _alignGroup.size();
    _alignGroup.emplace_back(strategy, data);

    if (!data.empty()) {
        _startNodeToGroupIndex[data.front()] = pos;
    }
}

void
AlignAnalyzer::AnalyzeContinuousLocalOrAssign(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    std::size_t lastLine = 0;
    std::vector<std::size_t> group;
    for (auto stmt: children) {
        auto kind = stmt.GetSyntaxKind(t);
        if (group.empty()) {
            if (kind == LuaSyntaxNodeKind::LocalStatement
                || kind == LuaSyntaxNodeKind::AssignStatement) {
                group.push_back(stmt.GetIndex());
                lastLine = stmt.GetEndLine(t);
            }
            continue;
        }

        if (stmt.GetTokenKind(t) == TK_SHORT_COMMENT) {
            auto line = stmt.GetStartLine(t);
            if (line - lastLine > 2) {
                if (group.size() > 1) {
                    PushAlignGroup(AlignStrategy::AlignToEq, group);
                }
                group.clear();
            } else {
                lastLine = stmt.GetEndLine(t);
            }
            continue;
        }

        if (kind == LuaSyntaxNodeKind::LocalStatement
            || kind == LuaSyntaxNodeKind::AssignStatement) {
            auto line = stmt.GetStartLine(t);
            if (line - lastLine <= 2) {
                group.push_back(stmt.GetIndex());
            } else {
                if (group.size() > 1) {
                    PushAlignGroup(AlignStrategy::AlignToEq, group);
                }
                group.clear();
                group.push_back(stmt.GetIndex());
            }

            lastLine = stmt.GetEndLine(t);
        } else if (group.size() > 1) {
            PushAlignGroup(AlignStrategy::AlignToEq, group);
            group.clear();
        } else {
            group.clear();
        }
    }

    if (group.size() > 1) {
        PushAlignGroup(AlignStrategy::AlignToEq, group);
    }
}

bool IsRectField(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    return syntaxNode.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableField
           && !syntaxNode.GetChildToken('=', t).IsNull(t);
}

void AlignAnalyzer::AnalyzeContinuousTableField(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    std::size_t lastLine = 0;
    std::vector<std::size_t> group;
    for (auto field: children) {
        auto line = field.GetStartLine(t);
        if (line < lastLine) {
            continue;
        }
        if (group.empty()) {
            if (IsRectField(field, t)) {
                group.push_back(field.GetIndex());
                lastLine = field.GetEndLine(t);
            }
            continue;
        }

        auto tokenKind = field.GetTokenKind(t);
        if (tokenKind != 0) {
            if (tokenKind == TK_SHORT_COMMENT) {
                if (line - lastLine > 2) {
                    if (group.size() > 1) {
                        PushAlignGroup(AlignStrategy::AlignToEq, group);
                    }
                    group.clear();
                } else {
                    lastLine = field.GetEndLine(t);
                }
            }
            continue;
        }

        if (IsRectField(field, t)) {
            if (line == lastLine) {
                group.clear();
                lastLine++;
                continue;
            } else if (line - lastLine <= 2) {
                group.push_back(field.GetIndex());
            } else {
                if (group.size() > 1) {
                    PushAlignGroup(AlignStrategy::AlignToEq, group);
                }
                group.clear();
                group.push_back(field.GetIndex());
            }

            lastLine = field.GetEndLine(t);
        } else if (group.size() > 1) {
            PushAlignGroup(AlignStrategy::AlignToEq, group);
            group.clear();
        } else {
            group.clear();
        }
    }

    if (group.size() > 1) {
        PushAlignGroup(AlignStrategy::AlignToEq, group);
    }
}

void
AlignAnalyzer::ResolveAlignGroup(FormatState &f, std::size_t groupIndex, AlignGroup &group, const LuaSyntaxTree &t) {
    switch (group.Strategy) {
        case AlignStrategy::AlignToEq: {
            bool allowAlign = false;
            for (auto i: group.SyntaxGroup) {
                auto node = LuaSyntaxNode(i);
                auto eq = node.GetChildToken('=', t);
                if (eq.IsToken(t)) {
                    auto diff = eq.GetTextRange(t).StartOffset - eq.GetPrevToken(t).GetTextRange(t).EndOffset;
                    if (diff > 2) {
                        allowAlign = true;
                        break;
                    }
                }
            }
            if (allowAlign) {
                std::size_t maxDis = 0;
                for (auto i: group.SyntaxGroup) {
                    auto node = LuaSyntaxNode(i);
                    auto eq = node.GetChildToken('=', t);
                    if (eq.IsToken(t)) {
                        auto prev = eq.GetPrevToken(t);
                        auto newPos = prev.GetTextRange(t).EndOffset + 2 - node.GetTextRange(t).StartOffset;
                        if (newPos > maxDis) {
                            maxDis = newPos;
                        }
                        _resolveGroupIndex[eq.GetIndex()] = groupIndex;
                    }
                }
                group.AlignPos = maxDis;
            }
            break;
        }
        case AlignStrategy::AlignToFirst: {
            if (!f.IsNewLine()) {
                auto width = f.GetCurrentWidth();
                group.AlignPos = width;
                for (auto i: group.SyntaxGroup) {
                    auto node = LuaSyntaxNode(i);
                    auto leftToken = node.GetFirstToken(t);
                    _resolveGroupIndex[leftToken.GetIndex()] = groupIndex;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void AlignAnalyzer::AnalyzeExpressionList(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    std::vector<std::size_t> group;
    for (auto expr: syntaxNode.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t)) {
        group.push_back(expr.GetIndex());
    }
    PushAlignGroup(AlignStrategy::AlignToFirst, group);
}

void AlignAnalyzer::AnalyzeParamList(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    std::vector<std::size_t> group;
    for (auto token: syntaxNode.GetChildren(t)) {
        if (token.GetTokenKind(t) == TK_NAME || token.GetTokenKind(t) == TK_DOTS) {
            group.push_back(token.GetIndex());
        }
    }
    PushAlignGroup(AlignStrategy::AlignToFirst, group);
}


void AlignAnalyzer::AnalyzeIfStatement(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto ifExprs = syntaxNode.GetChildSyntaxNodes(LuaSyntaxNodeKind::BinaryExpression, t);
    std::vector<LuaSyntaxNode> logicOps;
    for (auto ifExpr: ifExprs) {
        auto ops = helper::CollectBinaryOperator(ifExpr, t, [](LuaTokenKind kind) {
            return kind == TK_AND || kind == TK_OR;
        });
        for (auto op: ops) {
            logicOps.push_back(op);
        }
    }

    std::size_t alignPos = 0;
    auto elseifs = syntaxNode.GetChildTokens(TK_ELSEIF, t);
    if (!elseifs.empty()) {
        alignPos = 7; // sizeof 'elseif '
    } else {
        alignPos = 3; // sizeof 'if '
        for (auto &n: logicOps) {
            if (n.GetTokenKind(t) == TK_AND) {
                alignPos = 4; // sizeof 'and '
                break;
            }
        }
    }

    std::vector<std::size_t> group;
    for (auto &n: logicOps) {
        auto nextToken = n.GetNextToken(t);
        if (nextToken.IsToken(t)) {
            group.push_back(nextToken.GetIndex());
        }
    }
    auto if_ = syntaxNode.GetChildToken(TK_IF, t);
    group.push_back(if_.GetNextToken(t).GetIndex());

    for (auto elseif_: elseifs) {
        group.push_back(elseif_.GetNextToken(t).GetIndex());
    }

    PushNormalAlignGroup(alignPos, group);
}

void AlignAnalyzer::PushNormalAlignGroup(std::size_t alignPos, std::vector<std::size_t> &data) {
    auto pos = _alignGroup.size();
    auto &alignGroup = _alignGroup.emplace_back(AlignStrategy::Normal, data);
    alignGroup.Resolve = true;
    alignGroup.AlignPos = alignPos;
    for (auto &n: data) {
        _resolveGroupIndex[n] = pos;
    }
}

void
AlignAnalyzer::AnalyzeContinuousSimilarCallArgs(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    std::size_t lastLine = 0;
    std::vector<std::size_t> group;
    for (auto stmt: children) {
        auto kind = stmt.GetSyntaxKind(t);
        if (group.empty()) {
            if (kind == LuaSyntaxNodeKind::ExpressionStatement) {
                group.push_back(stmt.GetIndex());
                lastLine = stmt.GetEndLine(t);
            }
            continue;
        }

        if (stmt.GetTokenKind(t) == TK_SHORT_COMMENT) {
            auto line = stmt.GetStartLine(t);
            if (line - lastLine > 2) {
                if (group.size() > 1) {
                    PushAlignGroup(AlignStrategy::AlignToEq, group);
                }
                group.clear();
            } else {
                lastLine = stmt.GetEndLine(t);
            }
            continue;
        }

        if (kind == LuaSyntaxNodeKind::LocalStatement
            || kind == LuaSyntaxNodeKind::AssignStatement) {
            auto line = stmt.GetStartLine(t);
            if (line - lastLine <= 2) {
                group.push_back(stmt.GetIndex());
            } else {
                if (group.size() > 1) {
                    PushAlignGroup(AlignStrategy::AlignToEq, group);
                }
                group.clear();
                group.push_back(stmt.GetIndex());
            }

            lastLine = stmt.GetEndLine(t);
        } else if (group.size() > 1) {
            PushAlignGroup(AlignStrategy::AlignToEq, group);
            group.clear();
        } else {
            group.clear();
        }
    }

    if (group.size() > 1) {
        PushAlignGroup(AlignStrategy::AlignToEq, group);
    }
}
