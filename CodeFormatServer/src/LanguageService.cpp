#include "CodeFormatServer/LanguageService.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "CodeFormatServer/VSCode.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeService/LuaFormatter.h"
#include "CodeFormatServer/LanguageClient.h"

using namespace std::placeholders;

LanguageService::LanguageService()
{
}

LanguageService::~LanguageService()
{
}

bool LanguageService::Initialize()
{
#define DynamicBind(Method, ParamClass)\
	[this](auto param) { auto inputParam = std::dynamic_pointer_cast<ParamClass>(param); return Method(inputParam);}

	_handles["initialize"] = DynamicBind(OnInitialize, vscode::InitializeParams);
	_handles["textDocument/didChange"] = DynamicBind(OnDidChange, vscode::DidChangeTextDocumentParams);
	_handles["textDocument/didOpen"] = DynamicBind(OnDidOpen, vscode::DidOpenTextDocumentParams);
	_handles["textDocument/formatting"] = DynamicBind(OnFormatting, vscode::DocumentFormattingParams);
	_handles["textDocument/didClose"] = DynamicBind(OnClose, vscode::DidCloseTextDocumentParams);
	_handles["updateEditorConfig"] = DynamicBind(OnEditorConfigUpdate, vscode::EditorConfigUpdateParams);
	_handles["textDocument/rangeFormatting"] = DynamicBind(OnRangeFormatting, vscode::DocumentRangeFormattingParams);
	_handles["textDocument/onTypeFormatting"] = DynamicBind(OnTypeFormatting, vscode::TextDocumentPositionParams);

	return true;
}

std::shared_ptr<vscode::Serializable> LanguageService::Dispatch(std::string_view method,
                                                                std::shared_ptr<vscode::Serializable> param)
{
	auto it = _handles.find(method);
	if (it != _handles.end())
	{
		return it->second(param);
	}
	return nullptr;
}

std::shared_ptr<vscode::InitializeResult> LanguageService::OnInitialize(std::shared_ptr<vscode::InitializeParams> param)
{
	auto result = std::make_shared<vscode::InitializeResult>();

	result->capabilities.documentFormattingProvider = true;
	result->capabilities.documentRangeFormattingProvider = true;

	vscode::DocumentOnTypeFormattingOptions typeOptions;

	typeOptions.firstTriggerCharacter = "\n";

	result->capabilities.documentOnTypeFormattingProvider = typeOptions;

	result->capabilities.textDocumentSync.change = vscode::TextDocumentSyncKind::Full;
	result->capabilities.textDocumentSync.openClose = true;  

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


	return result;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnDidChange(
	std::shared_ptr<vscode::DidChangeTextDocumentParams> param)
{
	std::string_view fileText;
	for (auto& content : param->contentChanges)
	{
		auto parser = LuaParser::LoadFromBuffer(std::move(content.text));
		parser->BuildAstWithComment();

		LanguageClient::GetInstance().CacheFile(param->textDocument.uri, parser);

		LanguageClient::GetInstance().DiagnosticFile(param->textDocument.uri);
	}

	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnDidOpen(
	std::shared_ptr<vscode::DidOpenTextDocumentParams> param)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(param->textDocument.text));
	parser->BuildAstWithComment();
	LanguageClient::GetInstance().CacheFile(param->textDocument.uri, parser);
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

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto& edit = result->edits.emplace_back();
	edit.newText = formatter.GetFormattedText();
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

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto& edit = result->edits.emplace_back();
	LuaFormatRange formattedRange(static_cast<int>(param->range.start.line), static_cast<int>(param->range.end.line));

	edit.newText = formatter.GetRangeFormattedText(formattedRange);
	edit.range = vscode::Range(
		vscode::Position(formattedRange.StartLine, formattedRange.StartCharacter),
		vscode::Position(formattedRange.EndLine + 1, formattedRange.EndCharacter)
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

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto& edit = result->edits.emplace_back();
	LuaFormatRange formattedRange(static_cast<int>(position.line) - 1, static_cast<int>(position.line) - 1);

	edit.newText = formatter.GetRangeFormattedText(formattedRange);
	edit.range = vscode::Range(
		vscode::Position(formattedRange.StartLine, formattedRange.StartCharacter),
		vscode::Position(formattedRange.EndLine + 1, formattedRange.EndCharacter)
	);
	return result;
}
