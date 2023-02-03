#pragma once

#include <vector>
#include <string>
#include "LuaParser/Ast/LuaSyntaxNodeKind.h"
#include "LuaParser/Lexer/LuaTokenKind.h"
#include "LuaParseError.h"

class LuaParser;

enum class MarkEventType {
    NodeStart,
    EatToken,
    Error,
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

        struct {
            LuaTokenKind TokenKind;
            LuaParserErrorKind ErrorKind;
        } Error;
    } U;
};

struct CompleteMarker;

struct Marker {
    explicit Marker(std::size_t pos);
    CompleteMarker Complete(LuaParser& p, LuaSyntaxNodeKind kind);
    void Undo(LuaParser& p);
    std::size_t Pos;
};

struct CompleteMarker {
    CompleteMarker();
    CompleteMarker(std::size_t start, std::size_t finish, LuaSyntaxNodeKind kind);

    Marker Precede(LuaParser& p);

    std::size_t Start;
    std::size_t Finish;
    LuaSyntaxNodeKind Kind;
};