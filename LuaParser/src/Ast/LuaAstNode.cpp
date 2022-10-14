#include "LuaParser/Ast/LuaAstNode.h"
#include "LuaParser/Ast/LuaAstTree.h"

LuaAstNode::LuaAstNode(std::size_t index)
        : _index(index) {

}

TextRange LuaAstNode::GetTextRange(const LuaAstTree &t) const {
    return TextRange(t.GetStartOffset(_index), t.GetEndOffset(_index));
}

std::size_t LuaAstNode::GetStartLine(const LuaAstTree &t) const {
    return t.GetFile().GetLine(t.GetStartOffset(_index));
}

std::size_t LuaAstNode::GetStartCol(const LuaAstTree &t) const {
    return t.GetFile().GetColumn(t.GetStartOffset(_index));
}

std::size_t LuaAstNode::GetEndLine(const LuaAstTree &t) const {
    return t.GetFile().GetLine(t.GetEndOffset(_index));
}

std::size_t LuaAstNode::GetEndCol(const LuaAstTree &t) const {
    return t.GetFile().GetColumn(t.GetEndOffset(_index));
}

std::string_view LuaAstNode::GetText(const LuaAstTree &t) const {
    return t.GetFile().Slice(t.GetStartOffset(_index), t.GetEndOffset(_index));
}

bool LuaAstNode::IsNode(const LuaAstTree &t) const {
    return t.IsNode(_index);
}

LuaAstNodeType LuaAstNode::GetType(const LuaAstTree &t) const {
    return t.GetNodeType(_index);
}

LuaTokenType LuaAstNode::GetTokenType(const LuaAstTree &t) const {
    return t.GetTokenType(_index);
}

LuaAstNode LuaAstNode::GetParent(const LuaAstTree &t) const {
    return LuaAstNode(0);
}

LuaAstNode LuaAstNode::GetSibling(const LuaAstTree &t) const {
    return LuaAstNode(t.GetSibling(_index));
}

LuaAstNode LuaAstNode::GetFirstChild(const LuaAstTree &t) const {
    return LuaAstNode(t.GetFirstChild(_index));
}

void LuaAstNode::ToNext(const LuaAstTree &t) {
    _index = t.GetSibling(_index);
}

bool LuaAstNode::IsNull(const LuaAstTree &t) const {
    return _index == 0;
}
