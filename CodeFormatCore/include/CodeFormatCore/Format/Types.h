#pragma once

#include "CodeFormatCore/Config/LuaStyleEnum.h"
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Types/TextRange.h"

struct IndentState {
    IndentState(LuaSyntaxNode node, std::size_t space, std::size_t tab)
        : SyntaxNode(node),
          SpaceSize(space),
          TabSize(tab) {}

    LuaSyntaxNode SyntaxNode;
    std::size_t SpaceSize;
    std::size_t TabSize;
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

struct FormatRange {
    FormatRange()
        : StartLine(0), StartCol(0),
          EndLine(0), EndCol(0) {}

    FormatRange(std::size_t startLine, std::size_t endLine)
        : StartLine(startLine), EndLine(endLine) {}

    std::size_t StartLine;
    std::size_t StartCol;

    std::size_t EndLine;
    std::size_t EndCol;
};

struct IndexRange {
    explicit IndexRange(std::size_t startIndex = 0, std::size_t endIndex = 0)
        : StartIndex(startIndex),
          EndIndex(endIndex) {}


    std::size_t StartIndex;
    std::size_t EndIndex;
};

enum class FormatEvent {
    NodeExceedLinebreak
};
