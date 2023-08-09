#include "CodeFormatCore/Format/Analyzer/AlignAnalyzer.h"
#include "CodeFormatCore/Format/FormatState.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "SyntaxNodeHelper.h"
#include "Util/StringUtil.h"


AlignAnalyzer::AlignAnalyzer() {
}

void AlignAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    if (f.GetStyle().align_continuous_assign_statement != ContinuousAlign::None) {
                        AnalyzeContinuousLocalOrAssign(f, syntaxNode, t);
                    }
                    if (f.GetStyle().align_continuous_similar_call_args) {
                        AnalyzeContinuousSimilarCallArgs(f, syntaxNode, t);
                    }

                    break;
                }
                case LuaSyntaxNodeKind::TableFieldList: {
                    if (f.GetStyle().align_continuous_rect_table_field != ContinuousAlign::None) {
                        AnalyzeContinuousRectField(f, syntaxNode, t);
                    }
                    if (f.GetStyle().align_array_table != AlignArrayTable::None) {
                        AnalyzeContinuousArrayTableField(f, syntaxNode, t);
                    }
                    break;
                }
                case LuaSyntaxNodeKind::CallExpression: {
                    if (f.GetStyle().align_call_args) {
                        auto exprList = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                        auto exprs = exprList.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
                        auto symbolLine = exprList.GetPrevToken(t).GetEndLine(t);
                        bool sameLine = true;
                        for (auto expr: exprs) {
                            sameLine = sameLine && expr.GetStartLine(t) == symbolLine;
                        }
                        if (sameLine) {
                            break;
                        }
                        AnalyzeExpressionList(f, exprList, t);
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
                case LuaSyntaxNodeKind::SuffixedExpression: {
                    if (f.GetStyle().align_chain_expr != AlignChainExpr::None) {
                        AnalyzeChainExpr(f, syntaxNode, t);
                    }
                }
                default: {
                    break;
                }
            }
        } else {
            switch (syntaxNode.GetTokenKind(t)) {
                case TK_SHORT_COMMENT: {
                    if (f.GetStyle().align_continuous_inline_comment) {
                        AnalyzeInlineComment(f, syntaxNode, t);
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    for (auto &group: _inlineCommentGroup) {
        PushAlignGroup(AlignStrategy::AlignComment, group);
    }
}

void AlignAnalyzer::Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
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
            case AlignStrategy::AlignToEqWhenExtraSpace:
            case AlignStrategy::AlignToEqAlways: {
                resolve.SetRelativeIndentAlign(alignGroup.AlignPos);
                break;
            }
            case AlignStrategy::AlignToFirst: {
                resolve.SetIndent(alignGroup.AlignPos, IndentStrategy::Absolute);
                break;
            }
            case AlignStrategy::AlignComment: {
                resolve.SetAlign(alignGroup.AlignPos);
                break;
            }
            default: {
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

void AlignAnalyzer::AnalyzeContinuousLocalOrAssign(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    std::size_t lastLine = 0;
    std::vector<std::size_t> group;
    auto strategy = AlignStrategy::AlignToEqWhenExtraSpace;
    if (f.GetStyle().align_continuous_assign_statement == ContinuousAlign::Always) {
        strategy = AlignStrategy::AlignToEqAlways;
    }

    for (auto stmt: children) {
        auto kind = stmt.GetSyntaxKind(t);
        if (group.empty()) {
            if (kind == LuaSyntaxNodeKind::LocalStatement || kind == LuaSyntaxNodeKind::AssignStatement) {
                group.push_back(stmt.GetIndex());
                lastLine = stmt.GetEndLine(t);
            }
            continue;
        }

        if (stmt.GetTokenKind(t) == TK_SHORT_COMMENT) {
            auto line = stmt.GetStartLine(t);
            if (line - lastLine > f.GetStyle().align_continuous_line_space) {
                if (group.size() > 1) {
                    PushAlignGroup(strategy, group);
                }
                group.clear();
            } else {
                lastLine = stmt.GetEndLine(t);
            }
            continue;
        }

        if (kind == LuaSyntaxNodeKind::LocalStatement || kind == LuaSyntaxNodeKind::AssignStatement) {
            auto line = stmt.GetStartLine(t);
            if (line - lastLine <= f.GetStyle().align_continuous_line_space) {
                group.push_back(stmt.GetIndex());
            } else {
                if (group.size() > 1) {
                    PushAlignGroup(strategy, group);
                }
                group.clear();
                group.push_back(stmt.GetIndex());
            }

            lastLine = stmt.GetEndLine(t);
        } else if (group.size() > 1) {
            PushAlignGroup(strategy, group);
            group.clear();
        } else {
            group.clear();
        }
    }

    if (group.size() > 1) {
        PushAlignGroup(strategy, group);
    }
}

bool IsRectField(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    return syntaxNode.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableField && !syntaxNode.GetChildToken('=', t).IsNull(t);
}

void AlignAnalyzer::AnalyzeContinuousRectField(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    std::size_t lastLine = 0;
    std::vector<std::size_t> group;
    auto strategy = AlignStrategy::AlignToEqWhenExtraSpace;
    if (f.GetStyle().align_continuous_rect_table_field == ContinuousAlign::Always) {
        strategy = AlignStrategy::AlignToEqAlways;
    }

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
                if (line - lastLine > f.GetStyle().align_continuous_line_space) {
                    if (group.size() > 1) {
                        PushAlignGroup(strategy, group);
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
            } else if (line - lastLine <= f.GetStyle().align_continuous_line_space) {
                group.push_back(field.GetIndex());
            } else {
                if (group.size() > 1) {
                    PushAlignGroup(strategy, group);
                }
                group.clear();
                group.push_back(field.GetIndex());
            }

            lastLine = field.GetEndLine(t);
        } else if (group.size() > 1) {
            PushAlignGroup(strategy, group);
            group.clear();
        } else {
            group.clear();
        }
    }

    if (group.size() > 1) {
        PushAlignGroup(strategy, group);
    }
}

bool IsArrayTableField(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    if (syntaxNode.GetSyntaxKind(t) != LuaSyntaxNodeKind::TableField) {
        return false;
    }

    auto expr = syntaxNode.GetFirstChild(t);
    return expr.GetSyntaxKind(t) == LuaSyntaxNodeKind::TableExpression;
}

void AlignAnalyzer::AnalyzeContinuousArrayTableField(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto children = syntaxNode.GetChildren(t);
    std::vector<LuaSyntaxNode> arrayTable;
    std::size_t lastLine = 0;
    for (auto field: children) {
        if (field.GetTokenKind(t) == TK_SHORT_COMMENT || field.GetTokenKind(t) == TK_LONG_COMMENT) {
            continue;
        }

        if (IsArrayTableField(field, t) && field.IsSingleLineNode(t) && field.GetStartLine(t) > lastLine) {
            lastLine = field.GetStartLine(t);
            auto tableExpr = field.GetFirstChild(t);
            arrayTable.push_back(tableExpr);
        } else if (arrayTable.size() > 1) {
            AnalyzeArrayTableAlign(f, arrayTable, t);
            arrayTable.clear();
        }
    }

    if (arrayTable.size() > 1) {
        AnalyzeArrayTableAlign(f, arrayTable, t);
    }
}

void AlignAnalyzer::AnalyzeArrayTableAlign(FormatState &f, std::vector<LuaSyntaxNode> &arrayTable, const LuaSyntaxTree &t) {
    std::vector<std::vector<LuaSyntaxNode>> arrayTableFieldVec;
    std::size_t maxAlign = 0;
    auto &file = t.GetFile();
    for (auto &table: arrayTable) {
        if (file.CheckCurrentLineUnicodeBefore(table.GetTextRange(t).GetEndOffset())) {
            return;
        }

        auto tableFieldList = table.GetChildSyntaxNode(LuaSyntaxNodeKind::TableFieldList, t);
        auto tableFields = tableFieldList.GetChildSyntaxNodes(LuaSyntaxNodeKind::TableField, t);
        if (tableFields.size() > maxAlign) {
            maxAlign = tableFields.size();
        }
        arrayTableFieldVec.push_back(std::move(tableFields));
    }

    std::vector<std::size_t> group;
    std::size_t alignPos = 1;
    if (f.GetStyle().space_around_table_field_list) {
        alignPos++;
    }

    std::size_t elementLength = 0;
    for (std::size_t i = 0; i < maxAlign; i++) {
        for (auto &tableFieldArray: arrayTableFieldVec) {
            if (i < tableFieldArray.size()) {
                auto text = tableFieldArray[i].GetText(t);
                if (elementLength < text.size()) {
                    elementLength = text.size();
                }
                group.push_back(tableFieldArray[i].GetFirstToken(t).GetIndex());
            }
        }
        PushNormalAlignGroup(alignPos, group);
        alignPos += elementLength;
        if (i + 1 == maxAlign) {
            if (f.GetStyle().space_around_table_field_list) {
                alignPos++;
            }
        } else {
            if (f.GetStyle().space_after_comma) {
                alignPos++;
            }
        }

        elementLength = 0;
        group.clear();
    }

    if (f.GetStyle().align_array_table == AlignArrayTable::ContainCurly) {
        for (auto table: arrayTable) {
            group.push_back(table.GetChildToken('}', t).GetIndex());
        }
        PushNormalAlignGroup(alignPos, group);
    }
}

void AlignAnalyzer::ResolveAlignGroup(FormatState &f, std::size_t groupIndex, AlignGroup &group, const LuaSyntaxTree &t) {
    switch (group.Strategy) {
        case AlignStrategy::AlignToEqWhenExtraSpace: {
            bool allowAlign = false;
            auto &file = t.GetFile();
            for (auto i: group.SyntaxGroup) {
                auto node = LuaSyntaxNode(i);
                auto eq = node.GetChildToken('=', t);
                if (eq.IsToken(t)) {
                    auto diff = eq.GetTextRange(t).StartOffset - eq.GetPrevToken(t).GetTextRange(t).GetEndOffset();
                    if (diff > 2) {
                        allowAlign = true;
                    }
                    if (file.CheckCurrentLineUnicodeBefore(eq.GetTextRange(t).StartOffset)) {
                        return;
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
                        auto newPos = prev.GetTextRange(t).GetEndOffset() + 2 - node.GetTextRange(t).StartOffset;
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
        case AlignStrategy::AlignToEqAlways: {
            std::size_t maxDis = 0;
            auto &file = t.GetFile();
            for (auto i: group.SyntaxGroup) {
                auto node = LuaSyntaxNode(i);
                auto eq = node.GetChildToken('=', t);
                if (eq.IsToken(t)) {
                    auto prev = eq.GetPrevToken(t);
                    if (file.CheckCurrentLineUnicodeBefore(eq.GetTextRange(t).StartOffset)) {
                        return;
                    }
                    auto newPos = prev.GetTextRange(t).GetEndOffset() + 2 - node.GetTextRange(t).StartOffset;
                    if (newPos > maxDis) {
                        maxDis = newPos;
                    }
                    _resolveGroupIndex[eq.GetIndex()] = groupIndex;
                }
            }
            group.AlignPos = maxDis;
            break;
        }
        case AlignStrategy::AlignToFirst: {
            if (group.SyntaxGroup.empty()) {
                return;
            }
            LuaSyntaxNode firstNode(group.SyntaxGroup.front());
            if (!f.IsNewLine(firstNode, t)) {
                auto width = f.CurrentWidth();
                if (f.GetMode() == FormatState::Mode::Diagnostic) {
                    width = firstNode.GetStartCol(t);
                }

                group.AlignPos = width;
                for (auto i: group.SyntaxGroup) {
                    _resolveGroupIndex[i] = groupIndex;
                }
            }
            break;
        }
        case AlignStrategy::AlignComment: {
            std::size_t maxDis = 0;
            auto &file = t.GetFile();
            for (auto i: group.SyntaxGroup) {
                auto comment = LuaSyntaxNode(i);
                if (comment.IsToken(t)) {
                    if (file.CheckCurrentLineUnicodeBefore(comment.GetTextRange(t).StartOffset)) {
                        return;
                    }

                    auto prev = comment.GetPrevToken(t);
                    auto newPos =
                            file.GetColumn(prev.GetTextRange(t).GetEndOffset()) +
                            f.GetStyle().space_before_inline_comment +
                            1;
                    if (newPos > maxDis) {
                        maxDis = newPos;
                    }
                    _resolveGroupIndex[comment.GetIndex()] = groupIndex;
                }
            }
            group.AlignPos = maxDis;
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

// 需求真是复杂
void AlignAnalyzer::AnalyzeIfStatement(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto if_ = syntaxNode.GetChildToken(TK_IF, t);
    auto elseifs = syntaxNode.GetChildTokens(TK_ELSEIF, t);
    std::vector<std::size_t> group;

    // if 之后的表达式可以有多种对齐方式
    group.push_back(if_.GetNextToken(t).GetIndex());
    auto ifCondition = if_.GetNextSibling(t);
    std::vector<LuaSyntaxNode> logicOps;
    std::size_t ifAlignPos = 0;
    if (ifCondition.GetSyntaxKind(t) == LuaSyntaxNodeKind::BinaryExpression) {
        auto ifConditionLogicOps = helper::CollectBinaryOperator(ifCondition, t, [](LuaTokenKind kind) {
            return kind == TK_AND || kind == TK_OR;
        });

        for (auto op: ifConditionLogicOps) {
            if (op.GetPrevToken(t).GetEndLine(t) != op.GetStartLine(t)) {
                if (op.GetTokenKind(t) == TK_AND) {
                    ifAlignPos = std::max(ifAlignPos, std::size_t(4));
                } else if (op.GetTokenKind(t) == TK_OR) {
                    ifAlignPos = std::max(ifAlignPos, std::size_t(3));
                }
            }
        }
        logicOps = ifConditionLogicOps;
    }

    // 如果仅仅if语句
    if (elseifs.empty()) {
        if (ifAlignPos != 0) {
            for (auto &n: logicOps) {
                auto nextToken = n.GetNextToken(t);
                if (nextToken.IsToken(t)) {
                    group.push_back(nextToken.GetIndex());
                }
            }
            PushNormalAlignGroup(ifAlignPos, group);
        }
        return;
    }

    // 若有 elseif

    auto spaceAfterIf = if_.GetNextToken(t).GetStartCol(t) - if_.GetStartCol(t);
    if (spaceAfterIf == 3 && ifAlignPos == 0) {
        group.clear();
        logicOps.clear();
    }

    ifAlignPos = 7;

    for (auto elseif_: elseifs) {
        auto elseifCondition = elseif_.GetNextSibling(t);
        auto ops = helper::CollectBinaryOperator(elseifCondition, t, [](LuaTokenKind kind) {
            return kind == TK_AND || kind == TK_OR;
        });
        for (auto op: ops) {
            logicOps.push_back(op);
        }
    }

    for (auto &n: logicOps) {
        auto nextToken = n.GetNextToken(t);
        if (nextToken.IsToken(t)) {
            group.push_back(nextToken.GetIndex());
        }
    }

    PushNormalAlignGroup(ifAlignPos, group);
}

void AlignAnalyzer::AnalyzeChainExpr(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto parent = syntaxNode.GetParent(t);
    if (parent.GetSyntaxKind(t) != LuaSyntaxNodeKind::ExpressionStatement && f.GetStyle().align_chain_expr == AlignChainExpr::OnlyCallStmt) {
        return;
    }

    std::vector<std::size_t> group;
    for (auto indexExpr: syntaxNode.GetChildSyntaxNodes(LuaSyntaxNodeKind::IndexExpression, t)) {
        group.push_back(indexExpr.GetFirstToken(t).GetIndex());
    }
    PushAlignGroup(AlignStrategy::AlignToFirst, group);
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

void AlignAnalyzer::AnalyzeContinuousSimilarCallArgs(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto exprStmts = syntaxNode.GetChildSyntaxNodes(LuaSyntaxNodeKind::ExpressionStatement, t);
    std::size_t lastLine = 0;
    std::size_t prefixLen = 0;
    std::vector<LuaSyntaxNode> group;

    for (auto stmt: exprStmts) {
        auto suffix = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::SuffixedExpression, t);
        auto callexpr = suffix.GetLastChildSyntaxNode(LuaSyntaxNodeKind::CallExpression, t);
        auto isSimpleStmt = stmt.IsSingleLineNode(t);

        if (group.empty() && isSimpleStmt) {
            group.push_back(callexpr);
            prefixLen = callexpr.GetStartCol(t) - stmt.GetStartCol(t);
            lastLine = callexpr.GetEndLine(t);
            continue;
        }

        if (isSimpleStmt) {
            auto line = callexpr.GetStartLine(t);
            auto stmtPrefixLen = callexpr.GetStartCol(t) - stmt.GetStartCol(t);
            if (line - lastLine <= f.GetStyle().align_continuous_line_space && stmtPrefixLen == prefixLen) {
                group.push_back(callexpr);
            } else {
                if (group.size() > 1) {
                    AnalyzeSimilarCallAlign(f, group, prefixLen, t);
                }
                group.clear();
                group.push_back(callexpr);
            }
            prefixLen = stmtPrefixLen;
            lastLine = callexpr.GetEndLine(t);
        } else if (group.size() > 1) {
            AnalyzeSimilarCallAlign(f, group, prefixLen, t);
            group.clear();
        } else {
            group.clear();
        }
    }

    if (group.size() > 1) {
        AnalyzeSimilarCallAlign(f, group, prefixLen, t);
    }
}

void AlignAnalyzer::AnalyzeSimilarCallAlign(FormatState &f, std::vector<LuaSyntaxNode> &callExprs, std::size_t prefixLen, const LuaSyntaxTree &t) {
    std::vector<std::vector<LuaSyntaxNode>> argsVec;
    std::size_t maxAlign = 0;
    for (auto &callExpr: callExprs) {
        auto argExprList = callExpr.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
        auto args = argExprList.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
        if (args.size() > maxAlign) {
            maxAlign = args.size();
        }
        argsVec.push_back(std::move(args));
    }

    std::vector<std::size_t> group;
    std::size_t alignPos = prefixLen + 1;
    if (f.GetStyle().space_inside_function_call_parentheses) {
        alignPos++;
    }

    std::size_t elementLength = 0;
    for (std::size_t i = 0; i < maxAlign; i++) {
        for (auto &args: argsVec) {
            if (i < args.size()) {
                auto text = args[i].GetText(t);
                if (elementLength < text.size()) {
                    elementLength = text.size();
                }
                group.push_back(args[i].GetFirstToken(t).GetIndex());
            }
        }
        PushNormalAlignGroup(alignPos, group);
        alignPos += elementLength;
        if (i + 1 == maxAlign) {
            if (f.GetStyle().space_inside_function_call_parentheses) {
                alignPos++;
            }
        } else {
            if (f.GetStyle().space_after_comma) {
                alignPos += 2;
            } else {
                alignPos++;
            }
        }

        elementLength = 0;
        group.clear();
    }
}

void AlignAnalyzer::AnalyzeInlineComment(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    auto prevToken = syntaxNode.GetPrevToken(t);
    if (prevToken.IsNull(t)) {
        return;
    }
    auto currentLine = syntaxNode.GetStartLine(t);
    if (prevToken.GetEndLine(t) != currentLine) {
        return;
    }

    auto text = syntaxNode.GetText(t);
    if (string_util::StartWith(text, "---@")) {
        return;
    }

    // now it is inline comment
    if (_inlineCommentGroup.empty()) {
        _inlineCommentGroup.emplace_back();
    }
    auto &topGroup = _inlineCommentGroup.back();
    if (topGroup.empty()) {
        topGroup.push_back(syntaxNode.GetIndex());
        return;
    }

    auto lastComment = LuaSyntaxNode(topGroup.back());
    if (currentLine - lastComment.GetEndLine(t) > 2) {
        auto &newTopGroup = _inlineCommentGroup.emplace_back();
        newTopGroup.push_back(syntaxNode.GetIndex());
    } else {
        topGroup.push_back(syntaxNode.GetIndex());
    }
}
