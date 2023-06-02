#pragma once

#include "LuaCompile/Lib/LSP/LSP.h"
#include "LuaCompile/Lib/LineIndex/LineIndex.h"

class SourceUpdateEvent {
public:
    enum class Action {
        None,
        Add,
        Replace,
        Delete
    };

    static SourceUpdateEvent From(const LineIndex &lineIndex, const lsp::Range &range, std::string &&text);

    SourceUpdateEvent();

    Action UpdateAction;
    TextRange Range;
    std::string Text;
};
