#include "IfStmtSyntax.h"

IfStmtSyntax::IfStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

ExprSyntax IfStmtSyntax::GetIfCondition(const LuaSyntaxTree &t) const {
    return GetMember<ExprSyntax>(t);
}

BodySyntax IfStmtSyntax::GetIfBody(const LuaSyntaxTree &t) const {
    return GetMember<BodySyntax>(t);
}

BodySyntax IfStmtSyntax::GetElseClauseBody(const LuaSyntaxTree &t) const {
    auto elseToken = _node.GetChildToken(LuaTokenKind::TK_ELSE, t);
    for (auto child = elseToken.GetNextSibling(t); child.IsNull(t); child.ToNext(t)) {
        if (child.GetSyntaxKind(t) == LuaSyntaxNodeKind::Body) {
            return BodySyntax(child);
        }
    }
    return BodySyntax();
}

std::size_t IfStmtSyntax::GetElseIfNum(const LuaSyntaxTree &t) const {
    return _node.CountTokenChild(LuaTokenKind::TK_ELSEIF, t);
}

std::vector<std::pair<ExprSyntax, BodySyntax>> IfStmtSyntax::GetElseIfPairs(const LuaSyntaxTree &t) const {
    std::vector<std::pair<ExprSyntax, BodySyntax>> results;

    enum class FindState {
        None,
        ElseIfFounded,
    } state = FindState::None;

    for (auto child = _node.GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
        switch (child.GetTokenKind(t)) {
            case LuaTokenKind::TK_ELSEIF: {
                results.emplace_back();
                state = FindState::ElseIfFounded;
                continue;
            }
            case LuaTokenKind::TK_END:
            case LuaTokenKind::TK_ELSE: {
                goto endLoop;
            }
            default: {
                break;
            }
        }
        switch (state) {
            case FindState::ElseIfFounded: {
                if (ExprSyntax::CanCast(child.GetSyntaxKind(t))) {
                    results.back().first = ExprSyntax(child);
                } else if (BodySyntax::CanCast(child.GetSyntaxKind(t))) {
                    results.back().second = BodySyntax(child);
                }
                break;
            }
            default: {
                break;
            }
        }
    }
endLoop:
    return results;
}
