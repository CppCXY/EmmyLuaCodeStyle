#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "LuaCompile/Lib/LineIndex/LineIndex.h"
#include "LuaCompile/Lua/Define/LuaSyntaxError.h"
#include "SourceUpdateEvent.h"


class LuaSource {
public:
    static LuaSource From(std::string text);

    explicit LuaSource();

    void UpdateFile(std::string &&fileText);

    void ApplyUpdate(SourceUpdateEvent & event);

    std::string_view GetSource() const;

    std::string_view Slice(TextRange range) const;

    const LineIndex &GetLineIndex() const;

protected:

    std::string _source;
    LineIndex _lineIndex;
};