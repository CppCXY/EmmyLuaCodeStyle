#pragma once

#include <memory>
#include <functional>
#include <map>
#include "LSP.h"

class LanguageServer;

template <class T>
std::shared_ptr<T> MakeRequestObject(nlohmann::json json)
{
	auto object = std::make_shared<T>();
	object->Deserialize(json);

	return object;
}

class LSPHandle
{
public:
	using MessageHandle = std::function<std::shared_ptr<lsp::Serializable>(nlohmann::json)>;

	LSPHandle(LanguageServer* server);

	~LSPHandle();

	bool Initialize();

	std::shared_ptr<lsp::Serializable> Dispatch(std::string_view method,
                                                nlohmann::json params);

    void RefreshDiagnostic();
private:
	template <class ParamType,class ReturnType>
	void JsonProtocol(std::string_view method, std::shared_ptr<ReturnType>(LSPHandle::* handle)(std::shared_ptr<ParamType>))
	{
		_handles[std::string(method)] = [this, handle](auto jsonParams) {
			return (this->*handle)(MakeRequestObject<ParamType>(jsonParams));
		};
	}

	std::shared_ptr<lsp::InitializeResult> OnInitialize(std::shared_ptr<lsp::InitializeParams> params);

	std::shared_ptr<lsp::Serializable> OnInitialized(std::shared_ptr<lsp::Serializable> param);

	std::shared_ptr<lsp::Serializable> OnDidChange(std::shared_ptr<lsp::DidChangeTextDocumentParams> param);

	std::shared_ptr<lsp::Serializable> OnDidOpen(std::shared_ptr<lsp::DidOpenTextDocumentParams> param);
	
	std::shared_ptr<lsp::Serializable> OnFormatting(std::shared_ptr<lsp::DocumentFormattingParams> param);

	std::shared_ptr<lsp::Serializable> OnClose(std::shared_ptr<lsp::DidCloseTextDocumentParams> param);

	std::shared_ptr<lsp::Serializable> OnEditorConfigUpdate(std::shared_ptr<lsp::ConfigUpdateParams> param);

	std::shared_ptr<lsp::Serializable> OnRangeFormatting(std::shared_ptr<lsp::DocumentRangeFormattingParams> param);

	std::shared_ptr<lsp::Serializable> OnTypeFormatting(std::shared_ptr<lsp::TextDocumentPositionParams> param);

	std::shared_ptr<lsp::CodeActionResult> OnCodeAction(std::shared_ptr<lsp::CodeActionParams> param);

	std::shared_ptr<lsp::Serializable> OnExecuteCommand(std::shared_ptr<lsp::ExecuteCommandParams> param);

//	std::shared_ptr<lsp::Serializable> OnDidChangeWatchedFiles(std::shared_ptr<lsp::DidChangeWatchedFilesParams> param);
//
//	std::shared_ptr<lsp::CompletionList> OnCompletion(std::shared_ptr<lsp::CompletionParams> param);

	std::shared_ptr<lsp::Serializable> OnWorkspaceDidChangeConfiguration(std::shared_ptr<lsp::DidChangeConfigurationParams> param);

	std::shared_ptr<lsp::DocumentDiagnosticReport> OnTextDocumentDiagnostic(std::shared_ptr<lsp::DocumentDiagnosticParams> param);

	std::shared_ptr<lsp::WorkspaceDiagnosticReport> OnWorkspaceDiagnostic(std::shared_ptr<lsp::WorkspaceDiagnosticParams> param);

	std::map<std::string, MessageHandle, std::less<>> _handles;

    LanguageServer* _server;
};
