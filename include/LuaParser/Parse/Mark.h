#pragma once

#include <vector>
#include "LuaParser/Ast/LuaAstNodeType.h"
#include "LuaParser/Lexer/LuaTokenType.h"

class LuaParser;

enum class MarkEventType {
    NodeStart,
    EatToken,
    NodeEnd
};

struct MarkEvent {
    explicit MarkEvent(MarkEventType type = MarkEventType::NodeStart);

    MarkEventType Type;
    union {
        struct {
            std::size_t Parent;
            LuaAstNodeType Kind;
        } Start;
        struct {
            std::size_t Index;
            LuaTokenType Kind;
        } Token;
    } U;
};

struct CompleteMarker;

struct Marker {
    explicit Marker(std::size_t pos);
    CompleteMarker Complete(LuaParser& p, LuaAstNodeType kind);
    std::size_t Pos;
};

struct CompleteMarker {
    CompleteMarker();
    CompleteMarker(std::size_t start, std::size_t finish, LuaAstNodeType kind);

    Marker Precede(LuaParser& p);

    std::size_t Start;
    std::size_t Finish;
    LuaAstNodeType Kind;
};