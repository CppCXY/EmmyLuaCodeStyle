#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include "LuaSyntaxNodeKind.h"
#include "LuaParser/Types/TextRange.h"
#include "LuaParser/Lexer/LuaTokenKind.h"

class LuaSyntaxTree;

class LuaSyntaxNode {
public:
    explicit LuaSyntaxNode(std::size_t index);

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

    LuaSyntaxNode GetSibling(const LuaSyntaxTree &t) const;

    LuaSyntaxNode GetFirstChild(const LuaSyntaxTree &t) const;

    void ToNext(const LuaSyntaxTree &t);

    bool IsNull(const LuaSyntaxTree &t) const;

    std::vector<LuaSyntaxNode> GetDescendants(const LuaSyntaxTree &t);

    std::size_t GetIndex() const;

    LuaSyntaxNode ChildSyntaxNode(LuaSyntaxNodeKind kind) const;

    LuaSyntaxNode ChildToken(LuaTokenKind kind) const;
private:
    std::size_t _index;
};

