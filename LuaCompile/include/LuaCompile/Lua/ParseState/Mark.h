#pragma once

#include <string>
#include <vector>

#include "LuaCompile/Lua/Kind/LuaSyntaxNodeKind.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"

class ParseState;

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
            LuaSyntaxNodeKind Kind;
        } Start;
        struct {
            std::size_t Index;
            LuaTokenKind Kind;
        } Token;
    } U;
};

struct CompleteMarker;

struct Marker {
    explicit Marker(std::size_t pos);

    void PreComplete(ParseState& p, LuaSyntaxNodeKind kind);
    CompleteMarker Complete(ParseState& p, LuaSyntaxNodeKind kind);
    CompleteMarker Complete(ParseState& p);
    CompleteMarker UnComplete(ParseState& p, LuaSyntaxNodeKind kind);
    CompleteMarker UnComplete(ParseState& p);
    CompleteMarker Undo(ParseState& p);
    std::size_t Pos;
};

struct CompleteMarker {
    CompleteMarker();
    CompleteMarker(std::size_t start, std::size_t finish, LuaSyntaxNodeKind kind);

    Marker Precede(ParseState& p);

    bool IsNone() const;

    bool IsComplete() const;

    std::size_t Start;
    std::size_t Finish;
    LuaSyntaxNodeKind Kind;
};