#include "CodeService/Diagnostic/StyleDiagnostic.h"

StyleDiagnostic::StyleDiagnostic(LuaDiagnosticStyle &style) {

}

const LuaDiagnosticStyle &StyleDiagnostic::GetStyle() const {
    return _style;
}

void StyleDiagnostic::PushDiagnostic(DiagnosticType type,
                                     TextRange range,
                                     std::string_view message) {
    _diagnostics.emplace_back(type, range, message);
}

std::vector<LuaDiagnostic> &StyleDiagnostic::GetDiagnosticResults() {
    return _diagnostics;
}




