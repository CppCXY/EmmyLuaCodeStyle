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

LuaSyntaxNode LuaSyntaxNode::GetSibling(const LuaSyntaxTree &t) const {
    return LuaSyntaxNode(t.GetSibling(_index));
}

LuaSyntaxNode LuaSyntaxNode::GetFirstChild(const LuaSyntaxTree &t) const {
    return LuaSyntaxNode(t.GetFirstChild(_index));
}

void LuaSyntaxNode::ToNext(const LuaSyntaxTree &t) {
    _index = t.GetSibling(_index);
}

bool LuaSyntaxNode::IsNull(const LuaSyntaxTree &t) const {
    return _index == 0;
}

std::vector<LuaSyntaxNode> LuaSyntaxNode::GetDescendants(const LuaSyntaxTree &t) {
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

bool LuaSyntaxNode::IsToken(const LuaSyntaxTree &t) const {
    return t.IsToken(_index);
}

std::size_t LuaSyntaxNode::GetIndex() const{
    return _index;
}

std::size_t LuaSyntaxNode::ChildSyntaxNode(LuaSyntaxNodeKind kind) const {
    return _index;
}

std::size_t LuaSyntaxNode::ChildToken(LuaTokenKind kind) const {
    return _index;
}


