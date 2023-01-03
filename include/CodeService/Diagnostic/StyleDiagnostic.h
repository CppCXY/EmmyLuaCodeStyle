#pragma once

#include <string>
#include "DiagnosticType.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Config/LuaDiagnosticStyle.h"

class StyleDiagnostic {
public:
    explicit StyleDiagnostic(LuaDiagnosticStyle &style);

    const LuaDiagnosticStyle &GetStyle() const;

    void PushDiagnostic(DiagnosticType type,
                        TextRange range,
                        std::string_view message);
    std::vector<LuaDiagnostic>& GetDiagnosticResults();
private:
    LuaDiagnosticStyle _style;
    std::vector<LuaDiagnostic> _diagnostics;
};
