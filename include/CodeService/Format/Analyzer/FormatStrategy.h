#pragma once

enum class SpaceStrategy {
    None,
    Space,
    LineBreak,
};

enum class TokenStrategy {
    Origin,
    Remove,
    StringSingleQuote,
    StringDoubleQuote,
    RemoveCommentTrailSpace
};

enum IndentStrategy {
    None,
    Space,
    Tab,
    Mix
};

enum class LineBreakStrategy {
    MultiLine,

};