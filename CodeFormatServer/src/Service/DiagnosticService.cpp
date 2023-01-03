#include "DiagnosticService.h"
#include "CodeService/Diagnostic/StyleDiagnostic.h"
#include "CodeService/Format/FormatBuilder.h"
#include "LanguageServer.h"

DiagnosticService::DiagnosticService(LanguageServer *owner) : Service(owner) {

}

std::vector<lsp::Diagnostic>
DiagnosticService::Diagnostic(std::size_t fileId,
                              const LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle,
                              LuaDiagnosticStyle &diagnosticStyle) {
    StyleDiagnostic d(diagnosticStyle);
    FormatBuilder f(luaStyle);
    f.FormatAnalyze(luaSyntaxTree);
    f.Diagnostic(d, luaSyntaxTree);

    auto &results = d.GetDiagnosticResults();
    std::vector<lsp::Diagnostic> diagnostics;
    auto &vfs = _owner->GetVFS();
    auto lineIndex = vfs.GetLineIndexDB().Query(fileId).value();

    for (auto &result: results) {
        auto &diag = diagnostics.emplace_back();
        diag.message = result.Message;
        auto startLC = lineIndex->GetLineCol(result.Range.StartOffset);
        auto endLC = lineIndex->GetLineCol(result.Range.EndOffset);
        diag.range = lsp::Range(
                lsp::Position(startLC.Line, startLC.Col),
                lsp::Position(endLC.Line, endLC.Col)
        );

        switch (result.Type) {
            case DiagnosticType::Indent:
            case DiagnosticType::Space: {
                diag.severity = lsp::DiagnosticSeverity::Warning;
                diag.code = "style-check";
                diag.source = "EmmyLua";
                break;
            }
            case DiagnosticType::NameStyle: {
                diag.code = "name-style-check";
                diag.source = "EmmyLua";
                diag.severity = lsp::DiagnosticSeverity::Information;
                break;
            }
            case DiagnosticType::Spell: {
                diag.code = "spell-check";
                diag.source = "EmmyLua";
                diag.severity = lsp::DiagnosticSeverity::Information;
                break;
            }
            default: {
                break;
            }
        }

    }
    return diagnostics;
}

