#pragma once

#include "LuaCompile/Lua/Source/SourceUpdateEvent.h"
class LuaSyntaxTree;

class TreeUpdateEvent {
public:
    enum class Action {
        UpdateTree,
        OnlyUpdateToken,
//        UpdateFromOffset
    };

    static TreeUpdateEvent From(SourceUpdateEvent& sourceUpdateEvent, LuaSyntaxTree& t);

    TreeUpdateEvent();

    Action UpdateAction;

    SourceUpdateEvent SourceEvent;
private:
    static bool ContainSpace(std::string_view source);

    static bool OnlySpace(std::string_view source);

    void InitByAdd(SourceUpdateEvent &sourceUpdateEvent, LuaSyntaxTree &t);

    void InitByDelete(SourceUpdateEvent &sourceUpdateEvent, LuaSyntaxTree &t);

    void InitByReplace(SourceUpdateEvent &sourceUpdateEvent, LuaSyntaxTree &t);
};
