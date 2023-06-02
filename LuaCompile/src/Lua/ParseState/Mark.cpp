#include "LuaCompile/Lua/ParseState/Mark.h"
#include "LuaCompile/Lua/ParseState/ParseState.h"
#include <cstring>


MarkEvent::MarkEvent(MarkEventType type)
    : Type(type) {
    std::memset(&U, 0, sizeof(U));
}


Marker::Marker(std::size_t pos)
    : Pos(pos) {
}

void Marker::PreComplete(ParseState &p, LuaSyntaxNodeKind kind) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        events[Pos].U.Start.Kind = kind;
    }
}

CompleteMarker Marker::Complete(ParseState &p, LuaSyntaxNodeKind kind) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        events[Pos].U.Start.Kind = kind;
        auto finish = events.size();
        events.emplace_back(MarkEventType::NodeEnd);

        return CompleteMarker(Pos, finish, kind);
    }
    return CompleteMarker(0, 0, LuaSyntaxNodeKind::None);
}

CompleteMarker Marker::UnComplete(ParseState &p, LuaSyntaxNodeKind kind) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        events[Pos].U.Start.Kind = kind;
        auto finish = events.size();
        events.emplace_back(MarkEventType::NodeEnd);

        return CompleteMarker(Pos, finish, LuaSyntaxNodeKind::None);
    }
    return CompleteMarker(0, 0, LuaSyntaxNodeKind::None);
}

CompleteMarker Marker::Undo(ParseState &p) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        events[Pos].U.Start.Kind = LuaSyntaxNodeKind::None;
    }
    return CompleteMarker(0, 0, LuaSyntaxNodeKind::None);
}

CompleteMarker Marker::Complete(ParseState &p) {
    auto &events = p.GetEvents();
    if (Pos < events.size() && events[Pos].U.Start.Kind != LuaSyntaxNodeKind::None) {
        auto finish = events.size();
        events.emplace_back(MarkEventType::NodeEnd);

        return CompleteMarker(Pos, finish, events[Pos].U.Start.Kind);
    }
    return CompleteMarker(Pos, 0, LuaSyntaxNodeKind::Complete);
}

CompleteMarker Marker::UnComplete(ParseState &p) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        auto finish = events.size();
        events.emplace_back(MarkEventType::NodeEnd);

        return CompleteMarker(Pos, finish, LuaSyntaxNodeKind::None);
    }
    return CompleteMarker(0, 0, LuaSyntaxNodeKind::None);
}

CompleteMarker::CompleteMarker()
    : Start(0),
      Finish(0),
      Kind(LuaSyntaxNodeKind::None) {
}

CompleteMarker::CompleteMarker(std::size_t start, std::size_t finish, LuaSyntaxNodeKind kind)
    : Start(start),
      Finish(finish),
      Kind(kind) {
}

Marker CompleteMarker::Precede(ParseState &p) {
    auto m = p.Mark();
    auto &event = p.GetEvents().at(Start);
    if (event.Type == MarkEventType::NodeStart) {
        event.U.Start.Parent = m.Pos;
    }

    return m;
}

bool CompleteMarker::IsNone() const {
    return Kind == LuaSyntaxNodeKind::None;
}

bool CompleteMarker::IsComplete() const {
    return Kind != LuaSyntaxNodeKind::None;
}
