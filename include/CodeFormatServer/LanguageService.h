#pragma once

#include <memory>
#include <functional>
#include <map>
#include "CodeFormatServer/VSCode.h"

template <class T>
std::shared_ptr<T> MakeRequestObject(nlohmann::json json)
{
	auto object = std::make_shared<T>();
	object->Deserialize(json);

	return object;
}

class LanguageService
{
public:
	using MessageHandle = std::function<std::shared_ptr<vscode::Serializable>(nlohmann::json)>;

	LanguageService();

	~LanguageService();

	bool Initialize();

	std::shared_ptr<vscode::Serializable> Dispatch(std::string_view method,
	                                               nlohmann::json params);

private:
	template <class ParamType,class ReturnType>
	void JsonProtocol(std::string_view method, std::shared_ptr<ReturnType>(LanguageService::* handle)(std::shared_ptr<ParamType>))
	{
		_handles[std::string(method)] = [this, handle](auto jsonParams) {
			return (this->*handle)(MakeRequestObject<ParamType>(jsonParams));
		};
	}

	std::shared_ptr<vscode::InitializeResult> OnInitialize(std::shared_ptr<vscode::InitializeParams> param);

	std::shared_ptr<vscode::Serializable> OnDidChange(std::shared_ptr<vscode::DidChangeTextDocumentParams> param);

	std::shared_ptr<vscode::Serializable> OnDidOpen(std::shared_ptr<vscode::DidOpenTextDocumentParams> param);
	
	std::shared_ptr<vscode::Serializable> OnFormatting(std::shared_ptr<vscode::DocumentFormattingParams> param);

	std::shared_ptr<vscode::Serializable> OnClose(std::shared_ptr<vscode::DidCloseTextDocumentParams> param);

	std::shared_ptr<vscode::Serializable> OnEditorConfigUpdate(std::shared_ptr<vscode::EditorConfigUpdateParams> param);

	std::shared_ptr<vscode::Serializable> OnRangeFormatting(std::shared_ptr<vscode::DocumentRangeFormattingParams> param);

	std::shared_ptr<vscode::Serializable> OnTypeFormatting(std::shared_ptr<vscode::TextDocumentPositionParams> param);

	std::shared_ptr<vscode::CodeActionResult> OnCodeAction(std::shared_ptr<vscode::CodeActionParams> param);

	std::shared_ptr<vscode::Serializable> OnExecuteCommand(std::shared_ptr<vscode::ExecuteCommandParams> param);

	std::map<std::string, MessageHandle, std::less<>> _handles;
};

