#pragma once

#include "LuaNodeType.h"
#include "LuaParser/Lexer/LuaTokenType.h"
#include "LuaParser/Lexer/LuaToken.h"

enum class NodeOrTokenType {
    Node,
    Token
};

struct IncrementalToken {
    LuaTokenType Kind;
    std::size_t Start;
    std::size_t Length;
};

struct NodeOrToken {
    explicit NodeOrToken(LuaNodeType nodeKind)
            : Type(NodeOrTokenType::Node),
              Sibling(0),
              FirstChild(0),
              LastChild(0) {
        Data.NodeKind = nodeKind;
    }

    explicit NodeOrToken(LuaToken &token)
            : Type(NodeOrTokenType::Token),
              Parent(0),
              Sibling(0),
              FirstChild(0),
              LastChild(0) {
        Data.Token.Kind = token.TokenType;
        Data.Token.Start = token.Range.StartOffset;
        Data.Token.Length = token.Range.EndOffset - token.Range.StartOffset + 1;
    }

    NodeOrTokenType Type;
    std::size_t Parent;
    std::size_t Sibling;
    std::size_t FirstChild;
    std::size_t LastChild;
    union {
        LuaNodeType NodeKind;
        IncrementalToken Token;
    } Data;
};

