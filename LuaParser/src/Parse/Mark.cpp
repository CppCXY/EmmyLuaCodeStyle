#include "LuaParser/Parse/Mark.h"
#include "LuaParser/Parse/LuaParser.h"

MarkEvent::MarkEvent(MarkEventType type)
        : Type(type) {
    std::memset(&U, 0, sizeof(U));
}


Marker::Marker(std::size_t pos)
        : Pos(pos) {

}

CompleteMarker Marker::Complete(LuaParser &p, LuaAstNodeType kind) {
    auto &events = p.GetEvents();
    if (Pos < events.size()) {
        events[Pos].U.Start.Kind = kind;
        auto finish = events.size();
        events.emplace_back(MarkEventType::NodeEnd);

        return CompleteMarker(Pos, finish, kind);
    }
    return CompleteMarker(0, 0, LuaAstNodeType::None);
}

CompleteMarker::CompleteMarker()
        : Start(0),
          Finish(0),
          Kind(LuaAstNodeType::None) {

}

CompleteMarker::CompleteMarker(std::size_t start, std::size_t finish, LuaAstNodeType kind)
        : Start(start),
          Finish(finish),
          Kind(kind) {

}

Marker CompleteMarker::Precede(LuaParser &p) {
    auto m = p.Mark();
    auto event = p.GetEvents().at(Start);
    if (event.Type == MarkEventType::NodeStart) {
        event.U.Start.Parent = m.Pos;
    }

    return m;
}



