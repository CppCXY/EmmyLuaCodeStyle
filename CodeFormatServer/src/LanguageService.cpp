#include "CodeFormatServer/LanguageService.h"
#include "CodeFormatServer/VSCode.h"
#include "CodeService/LuaFormatOptions.h"
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
	result->capabilities.textDocumentSync.change = vscode::TextDocumentSyncKind::Full;
	result->capabilities.textDocumentSync.openClose = true;
	return result;
}

std::shared_ptr<vscode::Serializable> LanguageService::OnDidChange(
	std::shared_ptr<vscode::DidChangeTextDocumentParams> param)
{
	LuaFormatOptions options;

	std::shared_ptr<LuaParser> parser = LuaParser::LoadFromBuffer(std::move(param->contentChanges[0].text));
	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, options);
	formatter.BuildFormattedElement();

	auto diagnosisInfos = formatter.GetDiagnosisInfos();

	auto vscodeDiagnosis = std::make_shared<vscode::PublishDiagnosticsParams>();
	vscodeDiagnosis->uri = param->textDocument.uri;

	for (auto diagnosisInfo : diagnosisInfos)
	{
		auto& diagnosis = vscodeDiagnosis->diagnostics.emplace_back();
		diagnosis.message = diagnosisInfo.Message;
		diagnosis.range = vscode::Range(
			vscode::Position(
				diagnosisInfo.Range.Start.Line,
				diagnosisInfo.Range.Start.Character
			),
			vscode::Position(
				diagnosisInfo.Range.End.Line,
				diagnosisInfo.Range.End.Character
			));
		diagnosis.severity = vscode::DiagnosticSeverity::Warning;
	}

	LanguageClient::GetInstance().SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);

	return nullptr;
}
