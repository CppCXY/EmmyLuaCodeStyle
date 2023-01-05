#include "DiagnosticService.h"
#include "LanguageServer.h"
#include "CodeService/Diagnostic/DiagnosticBuilder.h"

DiagnosticService::DiagnosticService(LanguageServer *owner) : Service(owner) {

}

std::vector<lsp::Diagnostic>
DiagnosticService::Diagnostic(std::size_t fileId,
                              const LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle,
                              LuaDiagnosticStyle &diagnosticStyle) {
    DiagnosticBuilder d(luaStyle, diagnosticStyle);
    d.DiagnosticAnalyze(luaSyntaxTree);
    auto &results = d.GetDiagnosticResults(luaSyntaxTree);
    std::vector<lsp::Diagnostic> diagnostics;
    auto &vfs = _owner->GetVFS();
    auto vFile = vfs.GetVirtualFile(fileId);
    auto lineIndex = vFile.GetLineIndex(vfs);
    if (!lineIndex) {
        return diagnostics;
    }

    for (auto &result: results) {
        auto &diag = diagnostics.emplace_back();
        diag.message = result.Message;
        auto startLC = lineIndex->GetLineCol(result.Range.StartOffset);
        auto endLC = lineIndex->GetLineCol(result.Range.EndOffset);
        diag.range = lsp::Range(
                lsp::Position(startLC.Line, startLC.Col),
                lsp::Position(endLC.Line, endLC.Col)
        );

        diag.source = "EmmyLua";
        switch (result.Type) {
            case DiagnosticType::Indent: {
                diag.code = "space-check";
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::Space: {
                diag.code = "space-check";
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::StringQuote: {
                diag.code = "string-quote";
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::EndWithNewLine: {
                diag.code = "end-with-new-line";
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::Align: {
                diag.code = "code-align";
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::NameStyle: {
                diag.code = "name-style-check";
                diag.severity = lsp::DiagnosticSeverity::Information;
                break;
            }
            case DiagnosticType::Spell: {
                diag.code = "spell-check";
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

