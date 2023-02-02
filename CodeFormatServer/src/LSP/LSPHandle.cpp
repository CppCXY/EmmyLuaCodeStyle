#include "LSP/LSPHandle.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "LSP.h"
#include "CodeService/Config/LuaStyle.h"
#include "LanguageServer.h"
#include "Service/CodeActionService.h"
#include "Service/FormatService.h"
#include "Service/ConfigService.h"
#include "Service/CommandService.h"
#include "Util/Url.h"
#include "Util/format.h"
#include "LanguageServer.h"
#include "CodeService/Format/Types.h"
#include "Service/DiagnosticService.h"
#include "Config/ClientConfig.h"

using namespace std::placeholders;

LSPHandle::LSPHandle(LanguageServer *server)
        : _server(server) {
    Initialize();
}

LSPHandle::~LSPHandle() {
}

bool LSPHandle::Initialize() {
    JsonProtocol("initialize", &LSPHandle::OnInitialize);
    JsonProtocol("initialized", &LSPHandle::OnInitialized);
    JsonProtocol("textDocument/didChange", &LSPHandle::OnDidChange);
    JsonProtocol("textDocument/didOpen", &LSPHandle::OnDidOpen);
    JsonProtocol("textDocument/didClose", &LSPHandle::OnClose);
    JsonProtocol("config/editorconfig/update", &LSPHandle::OnEditorConfigUpdate);
    JsonProtocol("textDocument/formatting", &LSPHandle::OnFormatting);
    JsonProtocol("textDocument/rangeFormatting", &LSPHandle::OnRangeFormatting);
    JsonProtocol("textDocument/onTypeFormatting", &LSPHandle::OnTypeFormatting);
    JsonProtocol("textDocument/codeAction", &LSPHandle::OnCodeAction);
    JsonProtocol("workspace/executeCommand", &LSPHandle::OnExecuteCommand);
    JsonProtocol("workspace/didChangeConfiguration", &LSPHandle::OnWorkspaceDidChangeConfiguration);
    JsonProtocol("textDocument/diagnostic", &LSPHandle::OnTextDocumentDiagnostic);
    JsonProtocol("workspace/diagnostic", &LSPHandle::OnWorkspaceDiagnostic);
    return true;
}

std::shared_ptr<lsp::Serializable> LSPHandle::Dispatch(std::string_view method,
                                                       nlohmann::json params) {
    auto it = _handles.find(method);
    if (it != _handles.end()) {
        return it->second(params);
    }
    return nullptr;
}

std::shared_ptr<lsp::InitializeResult> LSPHandle::OnInitialize(std::shared_ptr<lsp::InitializeParams> params) {
    _server->InitializeService();

    auto result = std::make_shared<lsp::InitializeResult>();

    result->capabilities.documentFormattingProvider = true;
    result->capabilities.documentRangeFormattingProvider = true;

    lsp::DocumentOnTypeFormattingOptions typeOptions;

    typeOptions.firstTriggerCharacter = "\n";

    result->capabilities.documentOnTypeFormattingProvider = typeOptions;

    result->capabilities.textDocumentSync.change = lsp::TextDocumentSyncKind::Incremental;
    result->capabilities.textDocumentSync.openClose = true;

    result->capabilities.codeActionProvider = true;
    result->capabilities.executeCommandProvider.commands =
            _server->GetService<CommandService>()->GetCommands();

    result->capabilities.diagnosticProvider.identifier = "EmmyLuaCodeStyle";
    result->capabilities.diagnosticProvider.workspaceDiagnostics = false;
    result->capabilities.diagnosticProvider.interFileDependencies = false;

    auto &editorConfigFiles = params->initializationOptions.editorConfigFiles;
    for (auto &configFile: editorConfigFiles) {
        _server->GetService<ConfigService>()->LoadEditorconfig(configFile.workspace, configFile.path);
    }

    std::filesystem::path localePath = params->initializationOptions.localeRoot;
    localePath /= params->locale + ".json";

    if (std::filesystem::exists(localePath) && std::filesystem::is_regular_file(localePath)) {
        _server->GetService<ConfigService>()->LoadLanguageTranslator(localePath.string());
    }

    ClientConfig clientConfig;
    clientConfig.Deserialize(params->initializationOptions.vscodeConfig);
    _server->GetService<ConfigService>()->UpdateClientConfig(clientConfig);

//    if (!param->initializationOptions.extensionChars.empty()) {
//        for (auto &c: param->initializationOptions.extensionChars) {
//            LuaIdentify::AddIdentifyChar(c);
//        }
//    }

    auto dictionaryPath = params->initializationOptions.dictionaryPath;
    if (!dictionaryPath.empty()) {
        for (auto &path: dictionaryPath) {
            _server->GetService<DiagnosticService>()->GetSpellChecker()->LoadDictionary(path);
        }
    }
    return result;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnInitialized(std::shared_ptr<lsp::Serializable> param) {
    return nullptr;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnDidChange(
        std::shared_ptr<lsp::DidChangeTextDocumentParams> params) {
    if (params->contentChanges.size() == 1) {
        auto &content = params->contentChanges.front();
        if (content.range.has_value()) {
            _server->GetVFS().UpdateFile(params->textDocument.uri, content.range.value(),
                                         std::move(content.text));
        } else {
            _server->GetVFS().UpdateFile(params->textDocument.uri,
                                         std::move(content.text));
        }
    } else {
        _server->GetVFS().UpdateFile(params->textDocument.uri, params->contentChanges);
    }

    return nullptr;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnDidOpen(
        std::shared_ptr<lsp::DidOpenTextDocumentParams> params) {
    _server->GetVFS().UpdateFile(params->textDocument.uri, std::move(params->textDocument.text));
    return nullptr;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnFormatting(
        std::shared_ptr<lsp::DocumentFormattingParams> params) {
    auto result = std::make_shared<lsp::DocumentFormattingResult>();
    auto &vfs = _server->GetVFS();
    auto vFile = vfs.GetVirtualFile(params->textDocument.uri);
    if (vFile.IsNull()) {
        result->hasError = true;
        return result;
    }

    auto opSyntaxTree = vFile.GetSyntaxTree(vfs);
    if (!opSyntaxTree.has_value()) {
        result->hasError = true;
        return result;
    }

    auto &syntaxTree = opSyntaxTree.value();

    if (syntaxTree.HasError()) {
        result->hasError = true;
        return result;
    }

    LuaStyle &luaStyle = _server->GetService<ConfigService>()->GetLuaStyle(params->textDocument.uri);

    auto newText = _server->GetService<FormatService>()->Format(syntaxTree, luaStyle);
    auto lineIndex = vFile.GetLineIndex(vfs);
    auto totalLine = lineIndex->GetTotalLine();

    auto &edit = result->edits.emplace_back();
    edit.newText = std::move(newText);
    edit.range = lsp::Range(
            lsp::Position(0, 0),
            lsp::Position(totalLine + 1, 0)
    );
    return result;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnClose(
        std::shared_ptr<lsp::DidCloseTextDocumentParams> params) {
    _server->GetVFS().ClearFile(params->textDocument.uri);
    return nullptr;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnEditorConfigUpdate(
        std::shared_ptr<lsp::ConfigUpdateParams> params) {
    switch (params->type) {
        case lsp::FileChangeType::Created:
        case lsp::FileChangeType::Changed: {
            _server->GetService<ConfigService>()->LoadEditorconfig(params->source.workspace, params->source.path);
            break;
        }
        case lsp::FileChangeType::Delete: {
            _server->GetService<ConfigService>()->RemoveEditorconfig(params->source.workspace);
            break;
        }
    }

    RefreshDiagnostic();

    return nullptr;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnRangeFormatting(
        std::shared_ptr<lsp::DocumentRangeFormattingParams> params) {
    auto result = std::make_shared<lsp::DocumentFormattingResult>();
    auto &vfs = _server->GetVFS();
    auto vFile = vfs.GetVirtualFile(params->textDocument.uri);
    if (vFile.IsNull()) {
        result->hasError = true;
        return result;
    }

    auto opSyntaxTree = vFile.GetSyntaxTree(vfs);
    if (!opSyntaxTree.has_value()) {
        result->hasError = true;
        return result;
    }

    auto &syntaxTree = opSyntaxTree.value();

    if (syntaxTree.HasError()) {
        result->hasError = true;
        return result;
    }

    LuaStyle &luaStyle = _server->GetService<ConfigService>()->GetLuaStyle(params->textDocument.uri);

    FormatRange range;
    range.StartLine = params->range.start.line;
    range.EndLine = params->range.end.line;

    auto newText = _server->GetService<FormatService>()->RangeFormat(syntaxTree, luaStyle, range);

    auto &edit = result->edits.emplace_back();
    edit.newText = std::move(newText);
    edit.range = lsp::Range(
            lsp::Position(range.StartLine, range.StartCol),
            lsp::Position(range.EndLine + 1, 0)
    );
    return result;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnTypeFormatting(
        std::shared_ptr<lsp::TextDocumentPositionParams> params) {
    auto result = std::make_shared<lsp::DocumentFormattingResult>();
    auto &vfs = _server->GetVFS();
    auto vFile = vfs.GetVirtualFile(params->textDocument.uri);
    if (vFile.IsNull()) {
        result->hasError = true;
        return result;
    }

    auto opSyntaxTree = vFile.GetSyntaxTree(vfs);
    if (!opSyntaxTree.has_value()) {
        result->hasError = true;
        return result;
    }

    auto &syntaxTree = opSyntaxTree.value();
    LuaStyle &luaStyle = _server->GetService<ConfigService>()->GetLuaStyle(params->textDocument.uri);

    LuaTypeFormatOptions typeFormatOptions;
    auto typeFormatResults = _server->GetService<FormatService>()->TypeFormat(
            "\n",
            params->position.line,
            params->position.character,
            syntaxTree, luaStyle, typeFormatOptions);
    for (auto &formatResult: typeFormatResults) {
        auto &edit = result->edits.emplace_back();
        edit.newText = std::move(formatResult.Text);
        edit.range = lsp::Range(
                lsp::Position(formatResult.Range.StartLine, formatResult.Range.StartCol),
                lsp::Position(formatResult.Range.EndLine, formatResult.Range.EndCol)
        );
    }

    return result;
}

std::shared_ptr<lsp::CodeActionResult> LSPHandle::OnCodeAction(std::shared_ptr<lsp::CodeActionParams> param) {
    auto codeActionResult = std::make_shared<lsp::CodeActionResult>();

    for (auto &diagnostic: param->context.diagnostics) {
        _server->GetService<CodeActionService>()->Dispatch(diagnostic, param, codeActionResult);
    }
    return codeActionResult;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnExecuteCommand(
        std::shared_ptr<lsp::ExecuteCommandParams> params) {
    _server->GetService<CommandService>()->Dispatch(params->command, params);
    return nullptr;
}

std::shared_ptr<lsp::Serializable> LSPHandle::OnWorkspaceDidChangeConfiguration(
        std::shared_ptr<lsp::DidChangeConfigurationParams> params) {
    ClientConfig clientConfig;
    clientConfig.Deserialize(params->settings);
    _server->GetService<ConfigService>()->UpdateClientConfig(clientConfig);
    RefreshDiagnostic();
    return nullptr;
}

std::shared_ptr<lsp::DocumentDiagnosticReport> LSPHandle::OnTextDocumentDiagnostic(
        std::shared_ptr<lsp::DocumentDiagnosticParams> params) {
    auto report = std::make_shared<lsp::DocumentDiagnosticReport>();
    report->kind = lsp::DocumentDiagnosticReportKind::Full;

    auto &vfs = _server->GetVFS();
    auto opFileId = vfs.GetUriDB().Query(params->textDocument.uri);
    if (!opFileId.has_value()) {
        return report;
    }

    auto opSyntaxTree = vfs.GetVirtualFile(params->textDocument.uri).GetSyntaxTree(vfs);
    if (!opSyntaxTree.has_value()) {
        return report;
    }

    auto &syntaxTree = opSyntaxTree.value();

    if (syntaxTree.HasError()) {
        return report;
    }

    LuaStyle &luaStyle = _server->GetService<ConfigService>()->GetLuaStyle(params->textDocument.uri);

    auto diagnostics = _server->GetService<DiagnosticService>()->Diagnostic(
            opFileId.value(), syntaxTree, luaStyle
    );
    report->resultId = std::to_string(opFileId.value());

    report->items = std::move(diagnostics);
    return report;
}


std::shared_ptr<lsp::WorkspaceDiagnosticReport> LSPHandle::OnWorkspaceDiagnostic(
        std::shared_ptr<lsp::WorkspaceDiagnosticParams> params) {
    return nullptr;
}

void LSPHandle::RefreshDiagnostic() {
    _server->SendRequest("workspace/diagnostic/refresh", nullptr);
}