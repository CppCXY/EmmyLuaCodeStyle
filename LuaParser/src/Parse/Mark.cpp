#include "LuaParser/Parse/Mark.h"
#include <cstring>
#include "LuaParser/Parse/LuaParser.h"


MarkEvent::MarkEvent(MarkEventType type)
        : Type(type) {
    std::memset(&U, 0, sizeof(U));
}


Marker::Marker(std::size_t pos)
        : Pos(pos) {

}

CompleteMarker Marker::Complete(LuaParser &p, LuaSyntaxNodeKind kind) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        events[Pos].U.Start.Kind = kind;
        auto finish = events.size();
        events.emplace_back(MarkEventType::NodeEnd);

        return CompleteMarker(Pos, finish, kind);
    }
    return CompleteMarker(0, 0, LuaSyntaxNodeKind::None);
}

void Marker::Undo(LuaParser &p) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        events[Pos].U.Start.Kind = LuaSyntaxNodeKind::None;
    }
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

Marker CompleteMarker::Precede(LuaParser &p) {
    auto m = p.Mark();
    auto& event = p.GetEvents().at(Start);
    if (event.Type == MarkEventType::NodeStart) {
        event.U.Start.Parent = m.Pos;
    }

    return m;
}



