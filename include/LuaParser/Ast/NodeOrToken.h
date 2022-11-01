#pragma once

#include "LuaSyntaxNodeKind.h"
#include "LuaParser/Lexer/LuaTokenKind.h"
#include "LuaParser/Lexer/LuaToken.h"

enum class NodeOrTokenType {
    Node,
    Token
};

struct IncrementalToken {
    LuaTokenKind Kind;
    std::size_t Start;
    std::size_t Length;
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

    explicit NodeOrToken(LuaToken &token)
            : Type(NodeOrTokenType::Token),
              Parent(0),
              NextSibling(0),
              PrevSibling(0),
              FirstChild(0),
              LastChild(0) {
        Data.Token.Kind = token.TokenType;
        Data.Token.Start = token.Range.StartOffset;
        Data.Token.Length = token.Range.EndOffset - token.Range.StartOffset + 1;
    }

    NodeOrTokenType Type;
    std::size_t Parent;
    std::size_t NextSibling;
    std::size_t PrevSibling;
    std::size_t FirstChild;
    std::size_t LastChild;
    union {
        LuaSyntaxNodeKind NodeKind;
        IncrementalToken Token;
    } Data;
};

