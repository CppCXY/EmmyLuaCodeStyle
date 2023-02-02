#include "CodeActionService.h"

#include "CommandService.h"
#include "Util/Url.h"
#include "Util/format.h"
#include "LanguageServer.h"
#include "DiagnosticService.h"

CodeActionService::CodeActionService(LanguageServer *owner)
        : Service(owner) {
}

bool CodeActionService::Initialize() {
    CodeProtocol(DiagnosticType::Spell, "spell", &CodeActionService::Spell);
    CodeProtocol(DiagnosticType::Align, "code-align", &CodeActionService::Reformat);
    CodeProtocol(DiagnosticType::Indent, "code-indent", &CodeActionService::Reformat);
    CodeProtocol(DiagnosticType::Space, "whitespace", &CodeActionService::Reformat);
    CodeProtocol(DiagnosticType::StringQuote, "string-quote", &CodeActionService::Reformat);
    CodeProtocol(DiagnosticType::EndWithNewLine, "end-with-new-line");
    CodeProtocol(DiagnosticType::NameStyle, "name-style");

    return true;
}

void CodeActionService::Dispatch(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> params,
                                 std::shared_ptr<lsp::CodeActionResult> results) {
    if (diagnostic.code.empty()) {
        return;
    }

    auto it = _handles.find(diagnostic.code);
    if (it != _handles.end()) {
        it->second(diagnostic, params, results);
    }
}


std::string CodeActionService::GetCode(DiagnosticType code) {
    auto it = _diagnosticMap.find(code);
    if (it != _diagnosticMap.end()) {
        return it->second;
    }

    return "";
}

void CodeActionService::CodeProtocol(DiagnosticType type, std::string_view code, CodeActionHandleMember handle) {
    _diagnosticMap[type] = code;
    if (!handle) {
        return;
    }
    _handles[std::string(code)] = [this, handle](auto diagnostic, auto params, auto result) {
        return (this->*handle)(diagnostic, params, result);
    };
}

void CodeActionService::Reformat(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> param,
                                 std::shared_ptr<lsp::CodeActionResult> result) {
    auto &action = result->actions.emplace_back();
    std::string title = "Reformat current line";
    action.title = title;
    action.command.title = title;
    action.command.command = _owner->GetService<CommandService>()->GetCommand(CommandService::Command::Reformat);
    action.command.arguments.emplace_back(param->textDocument.uri);
    action.command.arguments.push_back(param->range.Serialize());

    action.kind = lsp::CodeActionKind::QuickFix;
}

void CodeActionService::Spell(lsp::Diagnostic &diagnostic, std::shared_ptr<lsp::CodeActionParams> param,
                              std::shared_ptr<lsp::CodeActionResult> result) {
    auto &originText = diagnostic.data;
    if (originText.empty()) {
        return;
    }

    // add to custom dictionary
    {
        auto &action = result->actions.emplace_back();

        action.title = util::format("Add '{}' to workspace dictionary", originText);
        action.command.title = action.title;
        action.command.command = _owner->GetService<CommandService>()->GetCommand(
                CommandService::Command::SpellAddDict);
        action.command.arguments.emplace_back(originText);

        action.kind = lsp::CodeActionKind::QuickFix;
    }

    auto spellChecker = _owner->GetService<DiagnosticService>()->GetSpellChecker();
    auto suggests = spellChecker->GetSuggests(originText);
    for (auto &suggest: suggests) {
        if (!suggest.Term.empty()) {
            if (result->actions.size() >= 15) {
                break;
            }
            auto &action = result->actions.emplace_back();
            auto &term = suggest.Term;

            action.title = term;
            action.command.title = term;
            action.command.command = _owner->GetService<CommandService>()->GetCommand(
                    CommandService::Command::SpellCorrect);
            action.command.arguments.emplace_back(param->textDocument.uri);
            action.command.arguments.push_back(diagnostic.range.Serialize());
            action.command.arguments.emplace_back(term);

            action.kind = lsp::CodeActionKind::QuickFix;
        }
    }
}
