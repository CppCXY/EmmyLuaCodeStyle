#include "CodeFormatServer/LanguageService.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "CodeFormatServer/VSCode.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeService/LuaFormatter.h"
#include "CodeFormatServer/LanguageClient.h"
#include "CodeFormatServer/Service/CodeFormatService.h"
#include "CodeFormatServer/Service/ModuleService.h"
#include "Util/Url.h"
#include "Util/format.h"

using namespace std::placeholders;

LanguageService::LanguageService()
{
}

LanguageService::~LanguageService()
{
}

bool LanguageService::Initialize()
{
	JsonProtocol("initialize", &LanguageService::OnInitialize);
	JsonProtocol("initialized", &LanguageService::OnInitialized);
	JsonProtocol("textDocument/didChange", &LanguageService::OnDidChange);
	JsonProtocol("textDocument/didOpen", &LanguageService::OnDidOpen);
	JsonProtocol("textDocument/didClose", &LanguageService::OnClose);
	JsonProtocol("updateEditorConfig", &LanguageService::OnEditorConfigUpdate);
	JsonProtocol("textDocument/formatting", &LanguageService::OnFormatting);
	JsonProtocol("textDocument/rangeFormatting", &LanguageService::OnRangeFormatting);
	JsonProtocol("textDocument/onTypeFormatting", &LanguageService::OnTypeFormatting);
	JsonProtocol("textDocument/codeAction", &LanguageService::OnCodeAction);
	JsonProtocol("workspace/executeCommand", &LanguageService::OnExecuteCommand);
	return true;
}

std::shared_ptr<vscode::Serializable> LanguageService::Dispatch(std::string_view method,
                                                                nlohmann::json params)
{
	auto it = _handles.find(method);
	if (it != _handles.end())
	{
		return it->second(params);
	}
	return nullptr;
}

std::shared_ptr<vscode::InitializeResult> LanguageService::OnInitialize(std::shared_ptr<vscode::InitializeParams> param)
{
	LanguageClient::GetInstance().InitializeService();

	auto result = std::make_shared<vscode::InitializeResult>();

	result->capabilities.documentFormattingProvider = true;
	result->capabilities.documentRangeFormattingProvider = true;

	vscode::DocumentOnTypeFormattingOptions typeOptions;

	typeOptions.firstTriggerCharacter = "\n";

	result->capabilities.documentOnTypeFormattingProvider = typeOptions;

	result->capabilities.textDocumentSync.change = vscode::TextDocumentSyncKind::Full;
	result->capabilities.textDocumentSync.openClose = true;

	result->capabilities.codeActionProvider = true;
	result->capabilities.executeCommandProvider.commands = {
		"emmylua.reformat.me",
		"emmylua.import.me"
	};

	auto& configFiles = param->initializationOptions.configFiles;
	for (auto& configFile : configFiles)
	{
		LanguageClient::GetInstance().UpdateOptions(configFile.workspace, configFile.path);
	}

	std::filesystem::path localePath = param->initializationOptions.localeRoot;
	localePath /= param->locale + ".json";

	if (std::filesystem::exists(localePath) && std::filesystem::is_regular_file(localePath))
	{
		std::fstream fin(localePath.string(), std::ios::in);

		if (fin.is_open())
		{
			std::stringstream s;
			s << fin.rdbuf();
			std::string jsonText = s.str();
			auto json = nlohmann::json::parse(jsonText);

			if (json.is_object())
			{
				std::map<std::string, std::string> languageMap;
				for (auto [key,value] : json.items())
				{
					languageMap.insert({key, value});
				}

				LanguageTranslator::GetInstance().SetLanguageMap(std::move(languageMap));
			}
		}
	}

	LanguageClient::GetInstance().SetRoot(param->rootPath);

	return result;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnInitialized(std::shared_ptr<vscode::Serializable> param)
{
	LanguageClient::GetInstance().UpdateAllDiagnosis();
	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnDidChange(
	std::shared_ptr<vscode::DidChangeTextDocumentParams> param)
{
	// std::string_view fileText;
	for (auto& content : param->contentChanges)
	{
		LanguageClient::GetInstance().ReleaseFile(param->textDocument.uri);

		LanguageClient::GetInstance().CacheFile(param->textDocument.uri, std::move(content.text));

		LanguageClient::GetInstance().DiagnosticFile(param->textDocument.uri);
	}

	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnDidOpen(
	std::shared_ptr<vscode::DidOpenTextDocumentParams> param)
{
	LanguageClient::GetInstance().ReleaseFile(param->textDocument.uri);
	LanguageClient::GetInstance().CacheFile(param->textDocument.uri, std::move(param->textDocument.text));
	LanguageClient::GetInstance().DiagnosticFile(param->textDocument.uri);
	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnFormatting(
	std::shared_ptr<vscode::DocumentFormattingParams> param)
{
	auto parser = LanguageClient::GetInstance().GetFileParser(param->textDocument.uri);

	auto totalLine = parser->GetTotalLine();

	auto result = std::make_shared<vscode::DocumentFormattingResult>();

	if (totalLine == 0)
	{
		result->hasError = true;
		return result;
	}

	auto options = LanguageClient::GetInstance().GetOptions(param->textDocument.uri);

	if (parser->HasError())
	{
		result->hasError = true;
		return result;
	}

	auto newText = LanguageClient::GetInstance().GetService<CodeFormatService>()->Format(parser, options);

	auto& edit = result->edits.emplace_back();
	edit.newText = std::move(newText);
	edit.range = vscode::Range(
		vscode::Position(0, 0),
		vscode::Position(totalLine + 1, 0)
	);
	return result;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnClose(
	std::shared_ptr<vscode::DidCloseTextDocumentParams> param)
{
	LanguageClient::GetInstance().ReleaseFile(param->textDocument.uri);

	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnEditorConfigUpdate(
	std::shared_ptr<vscode::EditorConfigUpdateParams> param)
{
	switch (param->type)
	{
	case vscode::EditorConfigUpdateType::Created:
	case vscode::EditorConfigUpdateType::Changed:
		{
			LanguageClient::GetInstance().UpdateOptions(param->source.workspace, param->source.path);
			break;
		}
	case vscode::EditorConfigUpdateType::Delete:
		{
			LanguageClient::GetInstance().RemoveOptions(param->source.workspace);
			break;
		}
	}

	LanguageClient::GetInstance().UpdateAllDiagnosis();

	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnRangeFormatting(
	std::shared_ptr<vscode::DocumentRangeFormattingParams> param)
{
	auto parser = LanguageClient::GetInstance().GetFileParser(param->textDocument.uri);

	auto result = std::make_shared<vscode::DocumentFormattingResult>();

	auto options = LanguageClient::GetInstance().GetOptions(param->textDocument.uri);

	if (parser->HasError())
	{
		result->hasError = true;
		return result;
	}

	LuaFormatRange formatRange(static_cast<int>(param->range.start.line), static_cast<int>(param->range.end.line));
	auto formatResult = LanguageClient::GetInstance().GetService<CodeFormatService>()->RangeFormat(
		formatRange, parser, options);

	auto& edit = result->edits.emplace_back();
	edit.newText = std::move(formatResult);
	edit.range = vscode::Range(
		vscode::Position(formatRange.StartLine, formatRange.StartCharacter),
		vscode::Position(formatRange.EndLine + 1, formatRange.EndCharacter)
	);
	return result;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnTypeFormatting(
	std::shared_ptr<vscode::TextDocumentPositionParams> param)
{
	auto parser = LanguageClient::GetInstance().GetFileParser(param->textDocument.uri);
	auto position = param->position;

	auto result = std::make_shared<vscode::DocumentFormattingResult>();
	if (parser->IsEmptyLine(static_cast<int>(position.line) - 1))
	{
		result->hasError = true;
		return result;
	}

	auto options = LanguageClient::GetInstance().GetOptions(param->textDocument.uri);

	if (parser->HasError())
	{
		result->hasError = true;
		return result;
	}


	LuaFormatRange formattedRange(static_cast<int>(position.line) - 1, static_cast<int>(position.line) - 1);

	auto formatResult = LanguageClient::GetInstance().GetService<CodeFormatService>()->RangeFormat(
		formattedRange, parser, options);

	auto& edit = result->edits.emplace_back();
	edit.newText = std::move(formatResult);
	edit.range = vscode::Range(
		vscode::Position(formattedRange.StartLine, formattedRange.StartCharacter),
		vscode::Position(formattedRange.EndLine + 1, formattedRange.EndCharacter)
	);
	return result;
}

std::shared_ptr<vscode::CodeActionResult> LanguageService::OnCodeAction(std::shared_ptr<vscode::CodeActionParams> param)
{
	auto range = param->range;
	auto uri = param->textDocument.uri;
	auto filePath = url::UrlToFilePath(uri);
	auto codeActionResult = std::make_shared<vscode::CodeActionResult>();

	if (LanguageClient::GetInstance().GetService<CodeFormatService>()->IsDiagnosticRange(filePath, range))
	{
		auto& action = codeActionResult->actions.emplace_back();
		std::string title = "reformat me";
		action.title = title;
		action.command.title = title;
		action.command.command = "emmylua.reformat.me";
		action.command.arguments.push_back(param->textDocument.uri);
		action.command.arguments.push_back(param->range.Serialize());

		action.kind = vscode::CodeActionKind::QuickFix;
	}
	else if (LanguageClient::GetInstance().GetService<ModuleService>()->IsDiagnosticRange(filePath, range))
	{
		auto modules = LanguageClient::GetInstance().GetService<ModuleService>()->GetImportModules(filePath, range);
		auto& action = codeActionResult->actions.emplace_back();
		std::string title = "import me";
		action.title = title;
		action.command.title = title;
		action.command.command = "emmylua.import.me";
		action.command.arguments.push_back(param->textDocument.uri);
		action.command.arguments.push_back(param->range.Serialize());
		for (auto& module : modules)
		{
			auto object = nlohmann::json::object();
			object["moduleName"] = module.ModuleName;
			object["path"] = module.FilePath;
			action.command.arguments.push_back(object);
		}

		action.kind = vscode::CodeActionKind::QuickFix;
	}
	return codeActionResult;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnExecuteCommand(
	std::shared_ptr<vscode::ExecuteCommandParams> param)
{
	auto result = std::make_shared<vscode::Serializable>();
	if (param->command == "emmylua.reformat.me")
	{
		if (param->arguments.size() < 2)
		{
			return result;
		}

		std::string uri = param->arguments[0];
		vscode::Range range;

		range.Deserialize(param->arguments[1]);

		auto parser = LanguageClient::GetInstance().GetFileParser(uri);

		auto applyParams = std::make_shared<vscode::ApplyWorkspaceEditParams>();

		auto options = LanguageClient::GetInstance().GetOptions(uri);

		if (parser->HasError())
		{
			return result;
		}

		auto it = applyParams->edit.changes.emplace(uri, std::vector<vscode::TextEdit>());
		auto& change = it.first->second;

		auto& edit = change.emplace_back();
		LuaFormatRange formattedRange(static_cast<int>(range.start.line), static_cast<int>(range.end.line));

		auto formatResult = LanguageClient::GetInstance().GetService<CodeFormatService>()->RangeFormat(
			formattedRange, parser, options);

		edit.newText = std::move(formatResult);

		edit.range = vscode::Range(
			vscode::Position(formattedRange.StartLine, formattedRange.StartCharacter),
			vscode::Position(formattedRange.EndLine + 1, formattedRange.EndCharacter)
		);

		LanguageClient::GetInstance().SendRequest("workspace/applyEdit", applyParams);
	}
	else if (param->command == "emmylua.import.me")
	{
		if (param->arguments.size() < 3)
		{
			return result;
		}

		std::string uri = param->arguments[0];
		vscode::Range range;

		range.Deserialize(param->arguments[1]);

		std::string moduleName = param->arguments[2];
		auto pos = moduleName.find_last_of('.');
		std::string moduleDefineName;

		if (pos == std::string_view::npos)
		{
			moduleDefineName = moduleName;
		}
		else
		{
			moduleDefineName = moduleName.substr(pos + 1);
		}

		std::string requireString = format("local {} = require(\"{}\")\n", moduleDefineName, moduleName);
		auto parser = LanguageClient::GetInstance().GetFileParser(uri);

		auto applyParams = std::make_shared<vscode::ApplyWorkspaceEditParams>();
		auto it = applyParams->edit.changes.emplace(uri, std::vector<vscode::TextEdit>());
		auto& change = it.first->second;

		auto& edit = change.emplace_back();
		LuaFormatRange formattedRange(static_cast<int>(range.start.line), static_cast<int>(range.end.line));

		edit.newText = requireString;

		edit.range = vscode::Range(
			vscode::Position(formattedRange.StartLine, formattedRange.StartCharacter),
			vscode::Position(formattedRange.EndLine + 1, formattedRange.EndCharacter)
		);

		LanguageClient::GetInstance().SendRequest("workspace/applyEdit", applyParams);
	}
	return result;
}
