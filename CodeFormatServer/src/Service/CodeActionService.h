#pragma once

#include<string_view>

#include "Service.h"

class CodeActionService : public Service {
public:
    LANGUAGE_SERVICE(CodeActionService);

    enum class DiagnosticCode {
        Spell,
        Reformat,
        Module
    };

    using CodeActionHandle = std::function<void(lsp::Diagnostic &diagnostic,
                                                std::shared_ptr<lsp::CodeActionParams> param,
                                                std::shared_ptr<lsp::CodeActionResult>)>;
    using CodeActionHandleMember = void (CodeActionService::*)(lsp::Diagnostic &,
                                                               std::shared_ptr<lsp::CodeActionParams>,
                                                               std::shared_ptr<lsp::CodeActionResult>);

    CodeActionService(LanguageServer *owner);

    bool Initialize() override;

    void Dispatch(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> param,
                  std::shared_ptr<lsp::CodeActionResult> results);

    std::string GetCode(DiagnosticCode code);

private:
    void CodeProtocol(DiagnosticCode code, CodeActionHandleMember handle);

    void Reformat(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> param,
                  std::shared_ptr<lsp::CodeActionResult> result);

    void Spell(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> param,
               std::shared_ptr<lsp::CodeActionResult> result);

    void Module(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> param,
                std::shared_ptr<lsp::CodeActionResult> result);

    std::map<std::string, CodeActionHandle> _handles;
};
