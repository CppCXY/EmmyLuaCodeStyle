#pragma once

#include "LuaSyntaxNodeKind.h"
#include "LuaParser/Lexer/LuaTokenKind.h"
#include "LuaParser/Lexer/LuaToken.h"

enum class NodeOrTokenType {
    Node,
    Token
};

struct IncrementalToken {
    IncrementalToken(LuaToken &token, std::size_t nodeIndex)
            : Kind(token.TokenType),
              Start(token.Range.StartOffset),
              Length(token.Range.EndOffset - token.Range.StartOffset + 1),
              NodeIndex(nodeIndex) {
    }

    LuaTokenKind Kind;
    std::size_t Start;
    std::size_t Length;
    std::size_t NodeIndex;
};

struct NodeOrToken {
    explicit NodeOrToken(LuaSyntaxNodeKind nodeKind)
            : Type(NodeOrTokenType::Node),
              Parent(0),
              NextSibling(0),
              PrevSibling(0),
              FirstChild(0),
              LastChild(0) {
        Data.NodeKind = nodeKind;
    }

    explicit NodeOrToken(std::size_t tokenIndex)
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
        LuaSyntaxNodeKind NodeKind;
        std::size_t TokenIndex;
    } Data;
};

