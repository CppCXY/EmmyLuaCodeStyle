#pragma once

#include "CodeService/Config/LuaStyle.h"
#include "CodeService/Config/LuaDiagnosticStyle.h"
#include "CodeService/Format/FormatState.h"
#include "DiagnosticType.h"

class DiagnosticBuilder {
public:
    DiagnosticBuilder(LuaStyle &style, LuaDiagnosticStyle &diagnosticStyle);

    void DiagnosticAnalyze(const LuaSyntaxTree &t);

    std::vector<LuaDiagnostic> &GetDiagnosticResults(const LuaSyntaxTree &t);

private:
    void DoDiagnosticResolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    void CodeStyleCheck(const LuaSyntaxTree &t);

    void NameStyleCheck(const LuaSyntaxTree &t);

    void SpellCheck(const LuaSyntaxTree &t);

    void PushDiagnostic(DiagnosticType type,
                        TextRange range,
                        std::string_view message);

    void ProcessSpaceDiagnostic(LuaSyntaxNode &node, LuaSyntaxNode &next,
                                const LuaSyntaxTree &t,
                                size_t shouldSpace);

    std::string GetAdditionalNote(LuaSyntaxNode &left, LuaSyntaxNode &right, const LuaSyntaxTree &t);

    FormatState _state;
    std::vector<LuaDiagnostic> _diagnostics;
};