#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"

LuaSyntaxNode::LuaSyntaxNode(std::size_t index)
        : _index(index) {

}

TextRange LuaSyntaxNode::GetTextRange(const LuaSyntaxTree &t) const {
    return TextRange(t.GetStartOffset(_index), t.GetEndOffset(_index));
}

std::size_t LuaSyntaxNode::GetStartLine(const LuaSyntaxTree &t) const {
    return t.GetFile().GetLine(t.GetStartOffset(_index));
}

std::size_t LuaSyntaxNode::GetStartCol(const LuaSyntaxTree &t) const {
    return t.GetFile().GetColumn(t.GetStartOffset(_index));
}

std::size_t LuaSyntaxNode::GetEndLine(const LuaSyntaxTree &t) const {
    return t.GetFile().GetLine(t.GetEndOffset(_index));
}

std::size_t LuaSyntaxNode::GetEndCol(const LuaSyntaxTree &t) const {
    return t.GetFile().GetColumn(t.GetEndOffset(_index));
}

std::string_view LuaSyntaxNode::GetText(const LuaSyntaxTree &t) const {
    return t.GetFile().Slice(t.GetStartOffset(_index), t.GetEndOffset(_index));
}

bool LuaSyntaxNode::IsNode(const LuaSyntaxTree &t) const {
    return t.IsNode(_index);
}

LuaSyntaxNodeKind LuaSyntaxNode::GetSyntaxKind(const LuaSyntaxTree &t) const {
    return t.GetNodeKind(_index);
}

LuaTokenKind LuaSyntaxNode::GetTokenKind(const LuaSyntaxTree &t) const {
    return t.GetTokenKind(_index);
}

LuaSyntaxNode LuaSyntaxNode::GetParent(const LuaSyntaxTree &t) const {
    return LuaSyntaxNode(t.GetParent(_index));
}

LuaSyntaxNode LuaSyntaxNode::GetNextSibling(const LuaSyntaxTree &t) const {
    return LuaSyntaxNode(t.GetNextSibling(_index));
}

LuaSyntaxNode LuaSyntaxNode::GetPrevSibling(const LuaSyntaxTree &t) const {
    return LuaSyntaxNode(t.GetPrevSibling(_index));
}

LuaSyntaxNode LuaSyntaxNode::GetFirstChild(const LuaSyntaxTree &t) const {
    return LuaSyntaxNode(t.GetFirstChild(_index));
}

void LuaSyntaxNode::ToNext(const LuaSyntaxTree &t) {
    _index = t.GetNextSibling(_index);
}

bool LuaSyntaxNode::IsNull(const LuaSyntaxTree &t) const {
    return _index == 0;
}

std::vector<LuaSyntaxNode> LuaSyntaxNode::GetDescendants(const LuaSyntaxTree &t) const {
    std::vector<LuaSyntaxNode> results;
    if (t.GetFirstChild(_index) == 0) {
        return results;
    }

    int accessIndex = -1;
    LuaSyntaxNode node = *this;
    do {
        if (node.IsNode(t)) {
            for (auto child = node.GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
                results.emplace_back(child);
            }
        }
        accessIndex++;
    } while (accessIndex < static_cast<int>(results.size()));
    return results;
}

std::vector<LuaSyntaxNode> LuaSyntaxNode::GetChildren(const LuaSyntaxTree &t) const {
    std::vector<LuaSyntaxNode> results;
    for (auto child = GetFirstChild(t); !child.IsNull(t); child.ToNext(t)) {
        results.push_back(child);
    }
    return results;
}

bool LuaSyntaxNode::IsToken(const LuaSyntaxTree &t) const {
    return t.IsToken(_index);
}

std::size_t LuaSyntaxNode::GetIndex() const {
    return _index;
}

LuaSyntaxNode LuaSyntaxNode::GetChildSyntaxNode(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const {
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetSyntaxKind(t) == kind) {
            break;
        }
    }
    return child;
}

LuaSyntaxNode LuaSyntaxNode::GetChildSyntaxNode(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const {
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (detail::multi_match::Match(kind, child.GetSyntaxKind(t))) {
            break;
        }
    }
    return child;
}

LuaSyntaxNode LuaSyntaxNode::GetChildToken(LuaTokenKind kind, const LuaSyntaxTree &t) const {
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetTokenKind(t) == kind) {
            break;
        }
    }
    return child;
}

std::vector<LuaSyntaxNode> LuaSyntaxNode::GetChildSyntaxNodes(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaSyntaxNode> results;
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetSyntaxKind(t) == kind) {
            results.push_back(child);
        }
    }
    return results;
}

std::vector<LuaSyntaxNode> LuaSyntaxNode::GetChildSyntaxNodes(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaSyntaxNode> results;
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (detail::multi_match::Match(kind, child.GetSyntaxKind(t))) {
            results.push_back(child);
        }
    }
    return results;
}

std::vector<LuaSyntaxNode> LuaSyntaxNode::GetChildTokens(LuaTokenKind kind, const LuaSyntaxTree &t) const {
    std::vector<LuaSyntaxNode> results;
    auto child = GetFirstChild(t);
    for (; !child.IsNull(t); child.ToNext(t)) {
        if (child.GetTokenKind(t) == kind) {
            results.push_back(child);
        }
    }
    return results;
}

bool LuaSyntaxNode::IsSingleLineNode(const LuaSyntaxTree &t) const {
    return GetStartLine(t) == GetEndLine(t);
}

LuaSyntaxNode LuaSyntaxNode::GetNextToken(const LuaSyntaxTree &t) const {
    auto p = *this;
    auto next = p.GetNextSibling(t);
    while (next.IsNull(t) && !p.IsNull(t)) {
        p = p.GetParent(t);
        next = p.GetNextSibling(t);
    }

    if (!next.IsNull(t)) {
        return next.GetFirstToken(t);
    }
    return next;
}

LuaSyntaxNode LuaSyntaxNode::GetFirstToken(const LuaSyntaxTree &t) const {
    return LuaSyntaxNode(t.GetFirstToken(_index));
}

