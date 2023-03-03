#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include "LuaSyntaxNodeKind.h"
#include "LuaParser/Types/TextRange.h"
#include "LuaParser/Lexer/LuaTokenKind.h"
#include "LuaSyntaxMultiKind.h"

class LuaSyntaxTree;

class LuaSyntaxNode {
public:
    explicit LuaSyntaxNode(std::size_t index = 0);

    TextRange GetTextRange(const LuaSyntaxTree &t) const;

    std::size_t GetStartLine(const LuaSyntaxTree &t) const;

    std::size_t GetStartCol(const LuaSyntaxTree &t) const;

    std::size_t GetEndLine(const LuaSyntaxTree &t) const;

    std::size_t GetEndCol(const LuaSyntaxTree &t) const;

    std::string_view GetText(const LuaSyntaxTree &t) const;

    bool IsNode(const LuaSyntaxTree &t) const;

    bool IsToken(const LuaSyntaxTree &t) const;

    LuaSyntaxNodeKind GetSyntaxKind(const LuaSyntaxTree &t) const;

    LuaTokenKind GetTokenKind(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetParent(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetNextSibling(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetPrevSibling(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetFirstChild(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetLastChild(const LuaSyntaxTree &t) const;

    void ToNext(const LuaSyntaxTree &t);

    void ToPrev(const LuaSyntaxTree &t);

    bool IsNull(const LuaSyntaxTree &t) const;

    bool IsEmpty(const LuaSyntaxTree &t) const;

    std::vector<LuaSyntaxNode> GetDescendants(const LuaSyntaxTree &t) const;

    std::vector<LuaSyntaxNode> GetChildren(const LuaSyntaxTree &t) const;

    std::size_t GetIndex() const;

    LuaSyntaxNode GetChildSyntaxNode(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetChildSyntaxNode(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetLastChildSyntaxNode(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetLastChildSyntaxNode(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetChildToken(LuaTokenKind kind, const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetChildToken(std::function<bool(LuaTokenKind)> predicated, const LuaSyntaxTree &t) const;

    std::vector<LuaSyntaxNode> GetChildSyntaxNodes(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const;

    std::vector<LuaSyntaxNode> GetChildSyntaxNodes(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const;

    std::vector<LuaSyntaxNode> GetChildTokens(LuaTokenKind kind, const LuaSyntaxTree &t) const;

    bool IsSingleLineNode(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetNextToken(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetPrevToken(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetFirstToken(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetLastToken(const LuaSyntaxTree &t) const;

    std::size_t GetFirstLineWidth(const LuaSyntaxTree &t) const;

    std::size_t CountTokenChild(LuaTokenKind kind, const LuaSyntaxTree &t);

    std::size_t CountNodeChild(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t);
private:
    std::size_t _index;
};

