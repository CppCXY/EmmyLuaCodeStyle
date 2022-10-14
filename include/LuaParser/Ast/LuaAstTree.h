#pragma once

#include <memory>
#include <vector>
#include <stack>
#include "LuaParser/File/LuaFile.h"
#include "LuaParser/Parse/Mark.h"
#include "LuaParser/Lexer/LuaToken.h"
#include "NodeOrToken.h"

class LuaAstTree {
public:
    LuaAstTree();

    void BuildTree(LuaParser &p);

    const LuaFile &GetFile() const;

    std::size_t GetStartOffset(std::size_t index) const;

    std::size_t GetEndOffset(std::size_t index) const;

    std::size_t GetSibling(std::size_t index) const;

    std::size_t GetFirstChild(std::size_t index) const;

    LuaAstNodeType GetNodeType(std::size_t index) const;

    LuaTokenType GetTokenType(std::size_t index) const;

    bool IsNode(std::size_t index) const;
private:
    void StartNode(LuaAstNodeType kind, LuaParser &p);

    void EatComments(LuaParser &p);

    void EatToken(LuaParser &p);

    void FinishNode(LuaParser &p);

    void BuildNode(LuaAstNodeType kind);

    void BuildToken(LuaToken &token);

    std::shared_ptr<LuaFile> _file;
    std::vector<NodeOrToken> _nodes;
    std::stack<std::size_t> _nodePosStack;
    std::size_t _tokenIndex;
};