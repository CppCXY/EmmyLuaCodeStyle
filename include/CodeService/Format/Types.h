#pragma once

#include "CodeService/Config/LuaStyleEnum.h"
#include "LuaParser/Types/TextRange.h"

struct IndentState {
    IndentState(LuaSyntaxNode node, std::size_t space, std::size_t tab)
            :
            SyntaxNode(node),
            SpaceSize(space),
            TabSize(tab) {}

    LuaSyntaxNode SyntaxNode;
    std::size_t SpaceSize;
    std::size_t TabSize;
};

struct FormatChange {
    FormatChange()
            : Start(0), Length(0) {}

    std::size_t Start;
    std::size_t Length;
    std::string NewString;
};

enum class TraverseEvent {
    Enter,
    Exit
};

struct Traverse {
    Traverse(LuaSyntaxNode n, TraverseEvent e)
            : Node(n), Event(e) {}

    LuaSyntaxNode Node;
    TraverseEvent Event;
};
