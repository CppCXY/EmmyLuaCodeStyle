#include "LuaCompile/Lua/Source/SourceUpdateEvent.h"

SourceUpdateEvent SourceUpdateEvent::From(const LineIndex &lineIndex, const lsp::Range &range, std::string &&text) {
    SourceUpdateEvent e;

    auto startOffset = lineIndex.GetOffset(LineCol(range.start.line, range.start.character));
    auto endOffset = lineIndex.GetOffset(LineCol(range.end.line, range.end.character));

    if (endOffset < startOffset) {
        return e;
    }

    e.Text = std::move(text);
    // for insert
    if (startOffset == endOffset) {
        e.UpdateAction = Action::Add;
        e.Range = TextRange(startOffset, 0);
    }
    // for delete
    else if (e.Text.empty()) {
        e.UpdateAction = Action::Delete;
        e.Range = TextRange(startOffset, endOffset - startOffset);
    }
    // for replace
    else {
        e.UpdateAction = Action::Replace;
        e.Range = TextRange(startOffset, endOffset - startOffset);
    }

    return e;
}

SourceUpdateEvent::SourceUpdateEvent()
    : UpdateAction(Action::None) {
}
