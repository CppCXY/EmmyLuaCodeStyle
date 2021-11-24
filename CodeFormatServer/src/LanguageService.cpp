#include "CodeFormatServer/LanguageService.h"
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

	typeOptions.firstTriggerCharacter = ";";

	typeOptions.moreTriggerCharacter = {"\"", "\'", ",", ":", "."};

	result->capabilities.documentOnTypeFormattingProvider = typeOptions;

	result->capabilities.textDocumentSync.change = vscode::TextDocumentSyncKind::Full;
	result->capabilities.textDocumentSync.openClose = true;

	auto& configFiles = param->initializationOptions.configFiles;
	for (auto& configFile : configFiles)
	{
		LanguageClient::GetInstance().UpdateOptions(configFile.workspace, configFile.path);
	}


	return result;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnDidChange(
	std::shared_ptr<vscode::DidChangeTextDocumentParams> param)
{
	for (auto& content : param->contentChanges)
	{
		LanguageClient::GetInstance().CacheFile(param->textDocument.uri, content.text);

		LanguageClient::GetInstance().DiagnosticFile(param->textDocument.uri);
	}
	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnDidOpen(
	std::shared_ptr<vscode::DidOpenTextDocumentParams> param)
{
	LanguageClient::GetInstance().CacheFile(param->textDocument.uri, param->textDocument.text);
	LanguageClient::GetInstance().DiagnosticFile(param->textDocument.uri);

	return nullptr;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnFormatting(
	std::shared_ptr<vscode::DocumentFormattingParams> param)
{
	auto text = LanguageClient::GetInstance().GetFile(param->textDocument.uri);

	auto lastOffset = text.size();

	auto result = std::make_shared<vscode::DocumentFormattingResult>();

	if (text.empty())
	{
		result->hasError = true;
		return result;
	}

	auto options = LanguageClient::GetInstance().GetOptions(param->textDocument.uri);

	std::shared_ptr<LuaParser> parser = LuaParser::LoadFromBuffer(std::move(text));
	parser->BuildAstWithComment();

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
		vscode::Position(parser->GetLine(lastOffset), parser->GetColumn(lastOffset))
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
	auto text = LanguageClient::GetInstance().GetFile(param->textDocument.uri);

	auto result = std::make_shared<vscode::DocumentFormattingResult>();

	if (text.empty())
	{
		result->hasError = true;
		return result;
	}

	auto options = LanguageClient::GetInstance().GetOptions(param->textDocument.uri);

	std::shared_ptr<LuaParser> parser = LuaParser::LoadFromBuffer(std::move(text));
	parser->BuildAstWithComment();

	if (parser->HasError())
	{
		result->hasError = true;
		return result;
	}

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto& edit = result->edits.emplace_back();
	LuaFormatRange formattedRange(param->range.start.line, param->range.end.line);

	edit.newText = formatter.GetRangeFormattedText(formattedRange);
	edit.range = vscode::Range(
		vscode::Position(formattedRange.StartLine, 0),
		vscode::Position(formattedRange.EndLine + 1, 0)
	);
	return result;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnTypeFormatting(
	std::shared_ptr<vscode::TextDocumentPositionParams> param)
{
	auto text = LanguageClient::GetInstance().GetFile(param->textDocument.uri);

	auto result = std::make_shared<vscode::DocumentFormattingResult>();

	if (text.empty())
	{
		result->hasError = true;
		return result;
	}

	auto options = LanguageClient::GetInstance().GetOptions(param->textDocument.uri);

	std::shared_ptr<LuaParser> parser = LuaParser::LoadFromBuffer(std::move(text));
	parser->BuildAstWithComment();

	// ºöÂÔ¼üÈë´íÎó
	// if (parser->HasError())
	// {
	// 	result->hasError = true;
	// 	return result;
	// }

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto& edit = result->edits.emplace_back();
	LuaFormatRange formattedRange(param->position.line, param->position.line);

	edit.newText = formatter.GetRangeFormattedText(formattedRange);
	edit.range = vscode::Range(
		vscode::Position(formattedRange.StartLine, 0),
		vscode::Position(formattedRange.EndLine + 1, 0)
	);
	return result;
}
