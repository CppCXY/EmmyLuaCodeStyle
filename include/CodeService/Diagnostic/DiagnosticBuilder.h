#pragma once

#include "CodeService/Config/LuaDiagnosticStyle.h"
#include "DiagnosticType.h"
#include "CodeService/Diagnostic/Spell/CodeSpellChecker.h"
#include "CodeService/Diagnostic/NameStyle/NameStyleChecker.h"
#include "CodeService/Config/LuaStyle.h"
#include "CodeService/Format/FormatState.h"

class DiagnosticBuilder {
public:
    DiagnosticBuilder(LuaStyle &style, LuaDiagnosticStyle &diagnosticStyle);

    void CodeStyleCheck(const LuaSyntaxTree &t);

    void NameStyleCheck(const LuaSyntaxTree &t);

    void SpellCheck(const LuaSyntaxTree &t, CodeSpellChecker& spellChecker);

    std::vector<LuaDiagnostic> GetDiagnosticResults(const LuaSyntaxTree &t);

    void PushDiagnostic(DiagnosticType type,
                        std::size_t leftIndex,
                        TextRange range,
                        std::string_view message,
                        std::string_view data = "");

    void PushDiagnostic(DiagnosticType type,
                        TextRange range,
                        std::string_view message,
                        std::string_view data = "");


    void ClearDiagnostic(std::size_t leftIndex);

    FormatState& GetState();
private:

    LuaDiagnosticStyle _diagnosticStyle;
    FormatState _state;
    std::map<std::size_t, LuaDiagnostic> _nextDiagnosticMap;
    std::vector<LuaDiagnostic> _diagnostics;
};