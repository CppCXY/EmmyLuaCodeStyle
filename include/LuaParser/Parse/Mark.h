#pragma once

#include <vector>
#include "LuaParser/Ast/LuaNodeType.h"
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
            LuaNodeType Kind;
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
    CompleteMarker Complete(LuaParser& p, LuaNodeType kind);
    std::size_t Pos;
};

struct CompleteMarker {
    CompleteMarker();
    CompleteMarker(std::size_t start, std::size_t finish, LuaNodeType kind);

    Marker Precede(LuaParser& p);

    std::size_t Start;
    std::size_t Finish;
    LuaNodeType Kind;
};