#pragma once

#include "CodeService/Config/LuaStyle.h"
#include "CodeService/Config/LuaDiagnosticStyle.h"
#include "CodeService/Format/FormatState.h"
#include "DiagnosticType.h"
#include "CodeService/Diagnostic/Spell/CodeSpellChecker.h"
#include "CodeService/Diagnostic/NameStyle/NameStyleChecker.h"

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

    FormatState& GetState();
private:
    void DoDiagnosticResolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    void ClearDiagnostic(std::size_t leftIndex);

    void ProcessSpaceDiagnostic(LuaSyntaxNode &node, LuaSyntaxNode &next,
                                size_t shouldSpace,
                                const LuaSyntaxTree &t);

    std::string GetAdditionalNote(LuaSyntaxNode &left, LuaSyntaxNode &right, const LuaSyntaxTree &t);

    void ProcessIndentDiagnostic(LuaSyntaxNode &node, IndentData indentData, const LuaSyntaxTree &t);

    FormatState _state;
    std::map<std::size_t, LuaDiagnostic> _nextDiagnosticMap;
    std::vector<LuaDiagnostic> _diagnostics;
};