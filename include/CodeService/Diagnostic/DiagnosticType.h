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
    LuaDiagnostic()
        : Type(DiagnosticType::None), Range(), Message() {}

    LuaDiagnostic(DiagnosticType type, TextRange range, std::string_view message, std::string_view data)
            : Type(type), Range(range), Message(message), Data(data) {}

    DiagnosticType Type;
    TextRange Range;
    std::string Message;
    std::string Data;
};
