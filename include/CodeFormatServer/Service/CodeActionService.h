#pragma once

#include<string_view>

#include "Service.h"

class CodeActionService : public Service
{
public:
	LANGUAGE_SERVICE(CodeActionService);

	enum class DiagnosticCode
	{
		Spell,
		Reformat,
		Module
	};

	using CodeActionHandle = std::function<void(vscode::Diagnostic& diagnostic,
	                                            std::shared_ptr<vscode::CodeActionParams> param,
	                                            std::shared_ptr<vscode::CodeActionResult>)>;
	using CodeActionHandleMember = void (CodeActionService::*)(vscode::Diagnostic&,
	                                                           std::shared_ptr<vscode::CodeActionParams>,
	                                                           std::shared_ptr<vscode::CodeActionResult>);

	CodeActionService(std::shared_ptr<LanguageClient> owner);

	bool Initialize() override;

	void Dispatch(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
	              std::shared_ptr<vscode::CodeActionResult> results);

	std::string GetCode(DiagnosticCode code);

private:
	void CodeProtocol(DiagnosticCode code, CodeActionHandleMember handle);

	void Reformat(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
	              std::shared_ptr<vscode::CodeActionResult> result);

	void Spell(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
	           std::shared_ptr<vscode::CodeActionResult> result);

	void Module(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
	            std::shared_ptr<vscode::CodeActionResult> result);

	std::map<std::string, CodeActionHandle> _handles;
};
