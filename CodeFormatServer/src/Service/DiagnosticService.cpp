#include "DiagnosticService.h"
#include "LanguageServer.h"
#include "CodeService/Diagnostic/DiagnosticBuilder.h"
#include "CodeActionService.h"
#include "ConfigService.h"

DiagnosticService::DiagnosticService(LanguageServer *owner)
        : Service(owner),
          _spellChecker(std::make_shared<CodeSpellChecker>()) {

}

std::vector<lsp::Diagnostic>
DiagnosticService::Diagnostic(std::size_t fileId,
                              const LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle) {
    LuaDiagnosticStyle& diagnosticStyle = _owner->GetService<ConfigService>()->GetDiagnosticStyle();

    DiagnosticBuilder d(luaStyle, diagnosticStyle);

    d.CodeStyleCheck(luaSyntaxTree);
    d.SpellCheck(luaSyntaxTree, *_spellChecker);
    d.NameStyleCheck(luaSyntaxTree);

    auto results = d.GetDiagnosticResults(luaSyntaxTree);
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
                lsp::Position(endLC.Line, endLC.Col + 1)
        );
        diag.data = result.Data;

        diag.source = "EmmyLua";
        diag.code = _owner->GetService<CodeActionService>()->GetCode(result.Type);
        switch (result.Type) {
            case DiagnosticType::Indent: {
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::Space: {
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::StringQuote: {
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::EndWithNewLine: {
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::Align: {
                diag.severity = lsp::DiagnosticSeverity::Warning;
                break;
            }
            case DiagnosticType::NameStyle: {
                diag.severity = lsp::DiagnosticSeverity::Information;
                break;
            }
            case DiagnosticType::Spell: {
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

std::shared_ptr<CodeSpellChecker> DiagnosticService::GetSpellChecker() {
    return _spellChecker;
}

