#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "LuaCompile/Lib/TextRange/TextRange.h"
#include "LuaCompile/Lua/Kind/LuaSyntaxMultiKind.h"
#include "LuaCompile/Lua/Kind/LuaSyntaxNodeKind.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"
#include "LuaSyntaxTree.h"

class LuaNodeOrToken {
public:
    explicit LuaNodeOrToken(std::size_t index = 0);

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

    LuaNodeOrToken GetParent(const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetNextSibling(const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetPrevSibling(const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetFirstChild(const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetLastChild(const LuaSyntaxTree &t) const;

    void ToNext(const LuaSyntaxTree &t);

    void ToPrev(const LuaSyntaxTree &t);

    bool IsNull(const LuaSyntaxTree &t) const;

    bool IsEmpty(const LuaSyntaxTree &t) const;

    std::vector<LuaNodeOrToken> GetDescendants(const LuaSyntaxTree &t) const;

    std::vector<LuaNodeOrToken> GetChildren(const LuaSyntaxTree &t) const;

    std::size_t GetIndex() const;

    LuaNodeOrToken GetChildSyntaxNode(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetChildSyntaxNode(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetChildToken(LuaTokenKind kind, const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetChildToken(std::function<bool(LuaTokenKind)> predicated, const LuaSyntaxTree &t) const;

    std::vector<LuaNodeOrToken> GetChildSyntaxNodes(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const;

    std::vector<LuaNodeOrToken> GetChildSyntaxNodes(LuaSyntaxMultiKind kind, const LuaSyntaxTree &t) const;

    std::vector<LuaNodeOrToken> GetChildTokens(LuaTokenKind kind, const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetNextToken(const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetPrevToken(const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetFirstToken(const LuaSyntaxTree &t) const;

    LuaNodeOrToken GetLastToken(const LuaSyntaxTree &t) const;

    std::size_t GetFirstLineWidth(const LuaSyntaxTree &t) const;

    std::size_t CountTokenChild(LuaTokenKind kind, const LuaSyntaxTree &t) const;

    std::size_t CountNodeChild(LuaSyntaxNodeKind kind, const LuaSyntaxTree &t) const;

    LuaNodeOrToken Ancestor(const LuaSyntaxTree &t, const std::function<bool(LuaSyntaxNodeKind kind, bool &conitnueFlag)> &predicate) const;

    LuaNodeOrToken FindChild(const LuaSyntaxTree &t, const std::function<bool(LuaSyntaxNodeKind kind)> &predicate) const;

    std::vector<LuaNodeOrToken> FindChildren(const LuaSyntaxTree &t, const std::function<bool(LuaSyntaxNodeKind kind)> &predicate) const;

private:
    std::size_t _index;
};
