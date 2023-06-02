#pragma once

#include <memory>
#include <stack>
#include <vector>

#include "InternalNodeOrToken.h"
#include "LuaCompile/Lua/Define/LuaSyntaxError.h"
#include "LuaCompile/Lua/Define/LuaToken.h"
#include "LuaCompile/Lua/Source/LuaSource.h"
#include "TreeUpdateEvent.h"

class LuaNodeOrToken;

class LuaSyntaxTree {
public:
    friend class LuaTreeBuilder;

    friend class LuaDocTreeBuilder;

    friend class LuaNodeOrToken;

    friend class TreeUpdateEvent;

    static std::unique_ptr<LuaSyntaxTree> ParseText(std::string &&text);

    explicit LuaSyntaxTree(LuaSource &&source);

    void Reset();

    const LuaSource &GetSource() const;

    LuaSource &GetSource();

    const std::vector<LuaNodeOrToken> &GetSyntaxNodes() const;

    std::vector<LuaNodeOrToken> GetTokens() const;

    LuaNodeOrToken GetRootNode() const;

    LuaNodeOrToken GetTokenBeforeOffset(std::size_t offset) const;

    LuaNodeOrToken GetTokenAtOffset(std::size_t offset) const;

    void ApplyUpdate(TreeUpdateEvent& treeUpdateEvent);

    std::string GetDebugView();

    std::string GetDebugSyntaxView();

    std::vector<LuaSyntaxError>& GetSyntaxErrors();
private:
    std::size_t GetStartOffset(std::size_t index) const;

    std::size_t GetEndOffset(std::size_t index) const;

    std::size_t GetNextSibling(std::size_t index) const;

    std::size_t GetPrevSibling(std::size_t index) const;

    std::size_t GetFirstChild(std::size_t index) const;

    std::size_t GetLastChild(std::size_t index) const;

    std::size_t GetFirstToken(std::size_t index) const;

    std::size_t GetLastToken(std::size_t index) const;

    std::size_t GetPrevToken(std::size_t index) const;

    std::size_t GetNextToken(std::size_t index) const;

    std::size_t GetParent(std::size_t index) const;

    LuaSyntaxNodeKind GetNodeKind(std::size_t index) const;

    LuaTokenKind GetTokenKind(std::size_t index) const;

    TextRange GetTokenRange(std::size_t index) const;

    bool IsNode(std::size_t index) const;

    bool IsToken(std::size_t index) const;

    LuaSource _source;
    std::vector<InternalNodeOrToken> _nodeOrTokens;
    std::vector<IncrementalToken> _tokens;
    std::vector<LuaSyntaxError> _errors;
};