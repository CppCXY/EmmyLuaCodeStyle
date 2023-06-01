#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"
#include "CodeFormatCore/Config/LanguageTranslator.h"

#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "CodeFormatCore/Diagnostic/CodeStyle/CodeStyleChecker.h"

DiagnosticBuilder::DiagnosticBuilder(LuaStyle &style, LuaDiagnosticStyle &diagnosticStyle)
        : _diagnosticStyle(diagnosticStyle),
          _state(FormatState::Mode::Diagnostic) {
    _state.SetFormatStyle(style);
    _state.SetDiagnosticStyle(diagnosticStyle);
}

std::vector<LuaDiagnostic> DiagnosticBuilder::GetDiagnosticResults(const LuaSyntaxTree &t) {
    for (auto &d: _nextDiagnosticMap) {
        _diagnostics.push_back(d.second);
    }

    return _diagnostics;
}

void
DiagnosticBuilder::PushDiagnostic(DiagnosticType type,
                                  std::size_t leftIndex,
                                  TextRange range,
                                  std::string_view message,
                                  std::string_view data) {
    _nextDiagnosticMap[leftIndex] = LuaDiagnostic(type, range, message, data);
}

void DiagnosticBuilder::PushDiagnostic(DiagnosticType type, TextRange range, std::string_view message,
                                       std::string_view data) {
    _diagnostics.emplace_back(type, range, message, data);
}

void DiagnosticBuilder::CodeStyleCheck(const LuaSyntaxTree &t) {
    if (!_diagnosticStyle.code_style_check) {
        return;
    }

    CodeStyleChecker checker;
    checker.Analyze(*this, t);
}

void DiagnosticBuilder::NameStyleCheck(const LuaSyntaxTree &t) {
    if (!_diagnosticStyle.name_style_check) {
        return;
    }

    NameStyleChecker nameStyleChecker;
    nameStyleChecker.Analyze(*this, t);
}

void DiagnosticBuilder::SpellCheck(const LuaSyntaxTree &t, CodeSpellChecker &spellChecker) {
    if (!_diagnosticStyle.spell_check) {
        return;
    }

    spellChecker.Analyze(*this, t);
}

void DiagnosticBuilder::ClearDiagnostic(std::size_t leftIndex) {
    auto it = _nextDiagnosticMap.find(leftIndex);
    if (it != _nextDiagnosticMap.end()) {
        _nextDiagnosticMap.erase(it);
    }
}

FormatState &DiagnosticBuilder::GetState() {
    return _state;
}
