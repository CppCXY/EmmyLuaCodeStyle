#pragma once

#include "LuaParser/Types/TextRange.h"

enum class DiagnosticType {
    None,
    Space,
    Align,
    MaxLineWidth,
    StringQuote,
    StatementLineSpace,
    EndWithNewLine,
    Indent,
    NameStyle,
    Spell
};

class LuaDiagnostic {
public:
    LuaDiagnostic(DiagnosticType type, TextRange range, std::string_view message)
            : Type(type), Range(range), Message(message) {}

    DiagnosticType Type;
    TextRange Range;
    std::string Message;
};