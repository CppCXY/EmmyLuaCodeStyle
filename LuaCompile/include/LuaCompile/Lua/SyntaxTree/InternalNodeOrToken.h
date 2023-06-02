#pragma once

#include "LuaCompile/Lua/Define/LuaToken.h"
#include "LuaCompile/Lua/Kind/LuaSyntaxNodeKind.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"

enum class NodeOrTokenType {
    Node,
    Token
};

struct IncrementalToken {
    IncrementalToken(LuaToken &token, std::size_t nodeIndex)
        : Kind(token.TokenType),
          Start(token.Range.StartOffset),
          Length(token.Range.Length),
          NodeIndex(nodeIndex) {
    }

    LuaTokenKind Kind;
    std::size_t Start;
    std::size_t Length;
    std::size_t NodeIndex;
};

struct InternalNodeOrToken {
    explicit InternalNodeOrToken(LuaSyntaxNodeKind nodeKind)
        : Type(NodeOrTokenType::Node),
          Parent(0),
          NextSibling(0),
          PrevSibling(0),
          FirstChild(0),
          LastChild(0) {
        Data.Node.Kind = nodeKind;
        Data.Node.SyntaxIndex = 0;
    }

    explicit InternalNodeOrToken(std::size_t tokenIndex)
        : Type(NodeOrTokenType::Token),
          Parent(0),
          NextSibling(0),
          PrevSibling(0),
          FirstChild(0),
          LastChild(0) {
        Data.TokenIndex = tokenIndex;
    }

    NodeOrTokenType Type;
    std::size_t Parent;
    std::size_t NextSibling;
    std::size_t PrevSibling;
    std::size_t FirstChild;
    std::size_t LastChild;
    union {
        struct {
            LuaSyntaxNodeKind Kind;
            std::size_t SyntaxIndex;
        } Node;
        std::size_t TokenIndex;
    } Data;
};
