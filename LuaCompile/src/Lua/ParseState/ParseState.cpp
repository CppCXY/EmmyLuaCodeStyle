#include "LuaCompile/Lua/ParseState/ParseState.h"

std::vector<MarkEvent> &ParseState::GetEvents() {
    return _events;
}

Marker ParseState::Mark() {
    auto pos = _events.size();
    _events.emplace_back();
    return Marker(pos);
}
