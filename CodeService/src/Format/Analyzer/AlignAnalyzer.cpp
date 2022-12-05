#include "CodeService/Format/Analyzer/AlignAnalyzer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeService/Format/FormatBuilder.h"


AlignAnalyzer::AlignAnalyzer() {

}

void AlignAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
    std::unordered_map<std::size_t, std::size_t> marks;
    for (auto syntaxNode: t.GetSyntaxNodes()) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::Block: {
                    if (f.GetStyle().align_continuous_assign_statement_to_equal) {
                        AnalyzeContinuousLocalOrAssign(f, syntaxNode, t);
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
                    auto exprlist = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                    if (f.GetStyle().align_call_args != AlignCallArgs::None) {

                    }
                    break;
                }
                case LuaSyntaxNodeKind::ParamList: {
                    if (f.GetStyle().align_function_params) {

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

void AlignAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
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
            case AlignStrategy::AlignToEq:{
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
AlignAnalyzer::AnalyzeContinuousLocalOrAssign(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
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

void AlignAnalyzer::AnalyzeContinuousTableField(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
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
AlignAnalyzer::ResolveAlignGroup(FormatBuilder &f, std::size_t groupIndex, AlignGroup &group, const LuaSyntaxTree &t) {
    switch (group.Strategy) {
        case AlignStrategy::AlignToEq: {
            bool allowAlign = false;
            for (auto i: group.SyntaxGroup) {
                auto node = LuaSyntaxNode(i);
                auto eq = node.GetChildToken('=', t);
                auto diff = eq.GetTextRange(t).StartOffset - eq.GetPrevToken(t).GetTextRange(t).EndOffset;
                if (diff > 2) {
                    allowAlign = true;
                    break;
                }
            }
            if (allowAlign) {
                std::size_t maxDis = 0;
                for (auto i: group.SyntaxGroup) {
                    auto node = LuaSyntaxNode(i);
                    auto eq = node.GetChildToken('=', t);
                    auto prev = eq.GetPrevToken(t);
                    auto newPos = prev.GetTextRange(t).EndOffset + 2 - node.GetTextRange(t).StartOffset;
                    if (newPos > maxDis) {
                        maxDis = newPos;
                    }
                    _resolveGroupIndex[eq.GetIndex()] = groupIndex;
                }
                group.AlignPos = maxDis;
            }
            break;
        }
        case AlignStrategy::AlignToFirst: {
            break;
        }
    }
}
