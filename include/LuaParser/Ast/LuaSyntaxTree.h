#pragma once

#include <memory>
#include <vector>
#include <stack>
#include "LuaParser/File/LuaFile.h"
#include "LuaParser/Parse/Mark.h"
#include "LuaParser/Lexer/LuaToken.h"
#include "NodeOrToken.h"
#include "LuaSyntaxNode.h"

class LuaSyntaxTree {
public:
    LuaSyntaxTree();

    void BuildTree(LuaParser &p);

    const LuaFile &GetFile() const;

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

    bool IsNode(std::size_t index) const;

    bool IsToken(std::size_t index) const;

    const std::vector<LuaSyntaxNode>& GetSyntaxNodes() const;

    std::vector<LuaSyntaxNode> GetTokens() const;

    LuaSyntaxNode GetRootNode() const;

    LuaSyntaxNode GetTokenBeforeOffset(std::size_t offset) const;

    LuaSyntaxNode GetTokenAtOffset(std::size_t offset) const;

    std::string GetDebugView();

    bool HasError() const;

    const std::vector<LuaParseError>& GetErrors() const;
private:
    void StartNode(LuaSyntaxNodeKind kind, LuaParser &p);

    void EatComments(LuaParser &p);

    void EatInlineComment(LuaParser &p);

    void EatToken(LuaParser &p);

    bool IsEatAllComment(LuaSyntaxNodeKind kind) const;

    void FinishNode(LuaParser &p);

    void BuildNode(LuaSyntaxNodeKind kind);

    void BuildToken(LuaToken &token);

    std::shared_ptr<LuaFile> _file;
    std::vector<NodeOrToken> _nodeOrTokens;
    std::vector<IncrementalToken> _tokens;
    std::vector<LuaSyntaxNode> _syntaxNodes;
    std::stack<std::size_t> _nodePosStack;
    std::size_t _tokenIndex;
    std::vector<LuaParseError> _errors;
};