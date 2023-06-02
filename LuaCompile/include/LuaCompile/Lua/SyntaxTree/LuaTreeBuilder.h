#pragma once

#include "LuaCompile/Lua/LuaParser/LuaParser.h"
#include "LuaSyntaxTree.h"

class LuaTreeBuilder {
public:
    LuaTreeBuilder();

    void BuildTree(LuaSyntaxTree &t, LuaParser &p);

private:
    void StartNode(LuaSyntaxNodeKind kind, LuaSyntaxTree &t, LuaParser &p);

    std::size_t CalculateTriviaCount(LuaSyntaxTree &t, LuaParser &p);

    std::size_t BindLeftComment(std::size_t startPos, LuaSyntaxTree &t, LuaParser &p);

    std::size_t BindRightComment(LuaSyntaxNodeKind kind, LuaSyntaxTree &t, LuaParser &p);

    void EatTriviaByCount(std::size_t count, LuaSyntaxTree &t, LuaParser &p);

    void EatTrivias(LuaSyntaxTree &t, LuaParser &p);

    void EatToken(LuaSyntaxTree &t, LuaParser &p);

    bool IsEatAllTrivia(LuaSyntaxNodeKind kind, LuaSyntaxTree &t) const;

    void FinishNode(LuaSyntaxTree &t, LuaParser &p);

    void BuildNode(LuaSyntaxNodeKind kind, LuaSyntaxTree &t);

    void BuildToken(LuaToken &token, LuaSyntaxTree &t);

    void BuildComments(std::vector<std::size_t>& group, LuaSyntaxTree &t, LuaParser &p);

    std::stack<std::size_t> _nodePosStack;
    std::size_t _tokenIndex;
};
