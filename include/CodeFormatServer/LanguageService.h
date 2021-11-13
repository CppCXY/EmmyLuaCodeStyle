#pragma once

#include <memory>
#include <functional>
#include <map>
#include "CodeFormatServer/VSCode.h"

class LanguageService
{
public:
	using MessageHandle = std::function<std::shared_ptr<vscode::Serializable>(std::shared_ptr<vscode::Serializable>)>;

	LanguageService();

	~LanguageService();

	bool Initialize();

	std::shared_ptr<vscode::Serializable> Dispatch(std::string_view method,
	                                               std::shared_ptr<vscode::Serializable> param);

private:
	std::shared_ptr<vscode::InitializeResult> OnInitialize(std::shared_ptr<vscode::InitializeParams> param);

	std::shared_ptr<vscode::Serializable> OnDidChange(std::shared_ptr<vscode::DidChangeTextDocumentParams> param);

	std::shared_ptr<vscode::Serializable> OnDidOpen(std::shared_ptr<vscode::DidOpenTextDocumentParams> param);
	
	std::shared_ptr<vscode::Serializable> OnFormatting(std::shared_ptr<vscode::DocumentFormattingParams> param);

	std::shared_ptr<vscode::Serializable> OnClose(std::shared_ptr<vscode::DidCloseTextDocumentParams> param);

	std::shared_ptr<vscode::Serializable> OnEditorConfigUpdate(std::shared_ptr<vscode::EditorConfigUpdateParams> param);

	std::map<std::string, MessageHandle, std::less<>> _handles;
};

