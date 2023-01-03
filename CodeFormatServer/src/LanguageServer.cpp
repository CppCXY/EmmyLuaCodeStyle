#include "LanguageServer.h"
#include <iterator>
#include <fstream>
#include "CodeService/Config/LuaEditorConfig.h"
#include "Util/format.h"
#include "Util/Url.h"
#include "Util/FileFinder.h"
#include "asio.hpp"
#include "Service/Service.h"
//#include "Service/CodeActionService.h"
//#include "Service/ModuleService.h"
#include "Service/FormatService.h"
//#include "Service/CompletionService.h"
//#include "Service/CommandService.h"
#include "Service/ConfigService.h"

LanguageServer::LanguageServer()
	: _idCounter(0),
	  _ioc(1),
      _lspHandle(this)
{
}

void LanguageServer::InitializeService()
{
	AddService<FormatService>();
//	AddService<ModuleService>();
//	AddService<CompletionService>();
//	AddService<CommandService>();
//	AddService<CodeActionService>();
    AddService<ConfigService>();

	for (auto& service : _services)
	{
		service->Initialize();
	}

	for (auto& service : _services)
	{
		service->Start();
	}
}

void LanguageServer::SetSession(std::shared_ptr<IOSession> session)
{
	_session = session;
}

std::shared_ptr<IOSession> LanguageServer::GetSession()
{
	return _session;
}

void LanguageServer::SendNotification(std::string_view method, std::shared_ptr<lsp::Serializable> param)
{
	auto json = nlohmann::json::object();
	json["jsonrpc"] = "2.0";
	json["method"] = method;
	if (param != nullptr)
	{
		json["params"] = param->Serialize();
	}
	else
	{
		json["params"] = nullptr;
	}

	if (_session)
	{
		auto dumpResult = json.dump();
		std::string message = util::format("Content-Length:{}\r\n\r\n", dumpResult.size());

		message.append(dumpResult);
		_session->Send(std::move(message));
	}
}

void LanguageServer::SendRequest(std::string_view method, std::shared_ptr<lsp::Serializable> param)
{
	auto json = nlohmann::json::object();
	json["jsonrpc"] = "2.0";
	json["method"] = method;
	json["id"] = GetRequestId();
	if (param) {
		json["params"] = param->Serialize();
	}
	else
	{
		json["params"] = nullptr;
	}
	if (_session)
	{
		auto dumpResult = json.dump();
		std::string message = util::format("Content-Length:{}\r\n\r\n", dumpResult.size());

		message.append(dumpResult);
		_session->Send(std::move(message));
	}
}

//
// void LanguageServer::DiagnosticFile(std::string_view uri)
// {
// 	auto filename = url::UrlToFilePath(uri);
// 	auto it = _fileMap.find(filename);
// 	if (it == _fileMap.end())
// 	{
// 		return;
// 	}
//
// 	auto vscodeDiagnosis = std::make_shared<lsp::PublishDiagnosticsParams>();
// 	vscodeDiagnosis->uri = uri;
//
// 	auto options = GetOptions(uri);
// 	if (!options->enable_check_codestyle)
// 	{
// 		SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
// 		return;
// 	}
//
// 	std::shared_ptr<LuaParser> parser = it->second->GetLuaParser();
//
// 	if (parser->HasError())
// 	{
// 		return;
// 	}
//
// 	// TODO move spell check to SpellService
// 	auto formatDiagnostic = GetService<FormatService>()->Diagnose(filename, parser, options);
// 	std::copy(formatDiagnostic.begin(), formatDiagnostic.end(), std::back_inserter(vscodeDiagnosis->diagnostics));
//
// 	if (_vscodeSettings.lintModule)
// 	{
// 		auto moduleDiagnosis = GetService<ModuleService>()->Diagnose(filename, parser);
// 		std::copy(moduleDiagnosis.begin(), moduleDiagnosis.end(), std::back_inserter(vscodeDiagnosis->diagnostics));
// 	}
//
// 	SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
// }

int LanguageServer::Run()
{
	if (_session)
	{
		int ret = _session->Run(*this);
		_session = nullptr;
		return ret;
	}
	return 1;
}

//std::shared_ptr<LuaCodeStyleOptions> LanguageServer::GetOptions(std::string_view uriOrFilename)
//{
//	std::string filename;
//	if (uriOrFilename.starts_with("file"))
//	{
//		filename = url::UrlToFilePath(uriOrFilename);
//	}
//	else
//	{
//		filename = std::string(uriOrFilename);
//	}
//
//	std::size_t matchLength = 0;
//	std::shared_ptr<LuaCodeStyleOptions> options = _defaultOptions;
//	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
//	{
//		if (filename.starts_with(it->first) && it->first.size() > matchLength)
//		{
//			matchLength = it->first.size();
//			options = it->second->Generate(filename);
//		}
//	}
//
//	return options;
//}
//
//void LanguageServer::UpdateCodeStyleOptions(std::string_view workspaceUri, std::string_view configPath)
//{
//	_configVersion++;
//	auto workspace = url::UrlToFilePath(workspaceUri);
//	for (auto& pair : _editorConfigVector)
//	{
//		if (pair.first == workspace)
//		{
//			pair.second = LuaEditorConfig::LoadFromFile(std::string(configPath));
//			pair.second->SetWorkspace(workspace);
//			pair.second->SetRootWorkspace(_root);
//			return;
//		}
//	}
//
//	_editorConfigVector.push_back({
//		workspace,
//		LuaEditorConfig::LoadFromFile(std::string(configPath))
//	});
//	_editorConfigVector.back().second->SetWorkspace(workspace);
//	_editorConfigVector.back().second->SetRootWorkspace(_root);
//}
//
//void LanguageServer::RemoveCodeStyleOptions(std::string_view workspaceUri)
//{
//	++_configVersion;
//	auto workspace = url::UrlToFilePath(workspaceUri);
//	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
//	{
//		if (it->first == workspace)
//		{
//			_editorConfigVector.erase(it);
//			break;
//		}
//	}
//}
//
// void LanguageServer::UpdateAllDiagnosis()
// {
// 	for (auto it : _fileMap)
// 	{
// 		auto uri = url::FilePathToUrl(it.first);
// 		DiagnosticFile(uri);
// 	}
// }


//
//void LanguageServer::SetRoot(std::string_view root)
//{
//	_root = root;
//}

asio::io_context& LanguageServer::GetIOContext()
{
	return _ioc;
}

uint64_t LanguageServer::GetRequestId()
{
	return ++_idCounter;
}

LSPHandle &LanguageServer::GetLSPHandle() {
    return _lspHandle;
}

VirtualFileSystem &LanguageServer::GetVFS() {
    return _vfs;
}
