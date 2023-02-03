#pragma once

#include <string_view>
#include <functional>

#include "Service.h"
#include "LSP/LSP.h"
#include "CodeService/Diagnostic/DiagnosticType.h"

class CodeActionService : public Service {
public:
    LANGUAGE_SERVICE(CodeActionService);

    using CodeActionHandle = std::function<void(lsp::Diagnostic &,
                                                std::shared_ptr<lsp::CodeActionParams>,
                                                std::shared_ptr<lsp::CodeActionResult>)>;
    using CodeActionHandleMember = void (CodeActionService::*)(lsp::Diagnostic &,
                                                               std::shared_ptr<lsp::CodeActionParams>,
                                                               std::shared_ptr<lsp::CodeActionResult>);

    CodeActionService(LanguageServer *owner);

    bool Initialize() override;

    void Dispatch(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> param,
                  std::shared_ptr<lsp::CodeActionResult> results);

    std::string GetCode(DiagnosticType type);

private:
    void CodeProtocol(DiagnosticType type, std::string_view code, CodeActionHandleMember handle = nullptr);

    void Reformat(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> params,
                  std::shared_ptr<lsp::CodeActionResult> result);

    void Spell(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> params,
               std::shared_ptr<lsp::CodeActionResult> result);

    std::map<DiagnosticType, std::string> _diagnosticMap;
    std::map<std::string, CodeActionHandle> _handles;
};
