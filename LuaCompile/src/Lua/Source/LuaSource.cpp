#include "LuaCompile/Lua/Source/LuaSource.h"

LuaSource LuaSource::From(std::string text) {
    LuaSource source;
    source.UpdateFile(std::move(text));
    return source;
}

LuaSource::LuaSource() {
}

std::string_view LuaSource::GetSource() const {
    return _source;
}

std::string_view LuaSource::Slice(TextRange range) const {
    std::string_view source = _source;
    return source.substr(range.StartOffset, range.Length);
}

void LuaSource::UpdateFile(std::string &&fileText) {
    _source = std::move(fileText);
    _lineIndex.Parse(_source);
}

void LuaSource::ApplyUpdate(SourceUpdateEvent &event) {
    auto oldSize = _source.size();
    auto newSize = oldSize + (event.Text.size() - event.Range.Length);
    if (newSize > _source.capacity()) {
        auto suitableCapacity = newSize + 4096;
        _source.reserve(suitableCapacity);
    }

    switch (event.UpdateAction) {
        case SourceUpdateEvent::Action::Add: {
            _source.insert(event.Range.StartOffset, event.Text);
            break;
        }
        case SourceUpdateEvent::Action::Replace: {
            _source.replace(event.Range.StartOffset, event.Range.Length, event.Text);
            break;
        }
        case SourceUpdateEvent::Action::Delete: {
            _source.erase(event.Range.StartOffset, event.Range.Length);
            break;
        }
        default: {
            return;
        }
    }

    _lineIndex.Parse(_source);
}

const LineIndex &LuaSource::GetLineIndex() const {
    return _lineIndex;
}
