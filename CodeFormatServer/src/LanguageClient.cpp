#include "CodeFormatServer/LanguageClient.h"
#include <iterator>
#include "CodeService/LuaEditorConfig.h"
#include "CodeService/LuaFormatter.h"
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "Util/Url.h"
#include "Util/FileFinder.h"
#include "asio.hpp"
#include "CodeFormatServer/Service/CodeActionService.h"
#include "CodeFormatServer/Service/ModuleService.h"
#include "CodeFormatServer/Service/CodeFormatService.h"
#include "CodeFormatServer/Service/CompletionService.h"
#include "CodeFormatServer/Service/CommandService.h"

LanguageClient& LanguageClient::GetInstance()
{
	static auto instance = std::make_shared<LanguageClient>();
	return *instance;
}

LanguageClient::LanguageClient()
	: _defaultOptions(std::make_shared<LuaCodeStyleOptions>()),
	  _idCounter(0),
	  _ioc(1)
{
}

void LanguageClient::InitializeService()
{
	AddService<CodeFormatService>();
	AddService<ModuleService>();
	AddService<CompletionService>();
	AddService<CommandService>();
	AddService<CodeActionService>();

	for (auto service : _services)
	{
		service->Initialize();
	}

	for (auto service : _services)
	{
		service->Start();
	}
}

void LanguageClient::SetSession(std::shared_ptr<IOSession> session)
{
	_session = session;
}

std::shared_ptr<IOSession> LanguageClient::GetSession()
{
	return _session;
}

void LanguageClient::SendNotification(std::string_view method, std::shared_ptr<vscode::Serializable> param)
{
	auto json = nlohmann::json::object();
	json["jsonrpc"] = "2.0";
	json["method"] = method;
	json["params"] = param->Serialize();

	if (_session)
	{
		auto dumpResult = json.dump();
		std::string message = Util::format("Content-Length:{}\r\n\r\n", dumpResult.size());

		message.append(dumpResult);
		_session->Send(std::move(message));
	}
}

void LanguageClient::SendRequest(std::string_view method, std::shared_ptr<vscode::Serializable> param)
{
	auto json = nlohmann::json::object();
	json["jsonrpc"] = "2.0";
	json["method"] = method;
	json["id"] = GetRequestId();
	json["params"] = param->Serialize();

	if (_session)
	{
		auto dumpResult = json.dump();
		std::string message = Util::format("Content-Length:{}\r\n\r\n", dumpResult.size());

		message.append(dumpResult);
		_session->Send(std::move(message));
	}
}

void LanguageClient::UpdateFile(std::string_view uri, vscode::Range range, std::string&& text)
{
	auto filename = url::UrlToFilePath(uri);
	auto it = _fileMap.find(filename);
	if (it == _fileMap.end())
	{
		auto virtualFile = std::make_shared<VirtualFile>(filename);
		virtualFile->UpdateFile(std::move(text));
		_fileMap[filename] = virtualFile;
	}
	else if (range.start.line == -1)
	{
		it->second->UpdateFile(std::move(text));
	}
	else
	{
		it->second->UpdateFile(range, std::move(text));
	}
}

void LanguageClient::UpdateFile(std::string_view uri, std::vector<vscode::TextDocumentContentChangeEvent>& changeEvent)
{
	auto filename = url::UrlToFilePath(uri);
	auto it = _fileMap.find(filename);
	if (it != _fileMap.end())
	{
		it->second->UpdateFile(changeEvent);
	}
}

void LanguageClient::ClearFile(std::string_view uri)
{
	auto filename = url::UrlToFilePath(uri);
	auto it = _fileMap.find(filename);
	if (it != _fileMap.end())
	{
		_fileMap.erase(it);
	}
}

void LanguageClient::DiagnosticFile(std::string_view uri)
{
	auto filename = url::UrlToFilePath(uri);
	auto it = _fileMap.find(filename);
	if (it == _fileMap.end())
	{
		return;
	}

	auto vscodeDiagnosis = std::make_shared<vscode::PublishDiagnosticsParams>();
	vscodeDiagnosis->uri = uri;

	auto options = GetOptions(uri);
	if (!options->enable_check_codestyle)
	{
		SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
		return;
	}

	std::shared_ptr<LuaParser> parser = it->second->GetLuaParser();

	if (parser->HasError())
	{
		return;
	}

	// TODO move spell check to SpellService
	auto formatDiagnostic = GetService<CodeFormatService>()->Diagnose(filename, parser, options);
	std::copy(formatDiagnostic.begin(), formatDiagnostic.end(), std::back_inserter(vscodeDiagnosis->diagnostics));

	if (_vscodeSettings.lintModule)
	{
		auto moduleDiagnosis = GetService<ModuleService>()->Diagnose(filename, parser);
		std::copy(moduleDiagnosis.begin(), moduleDiagnosis.end(), std::back_inserter(vscodeDiagnosis->diagnostics));
	}

	SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
}

void LanguageClient::DelayDiagnosticFile(std::string_view uri)
{
	auto stringUri = std::string(uri);
	auto task = std::make_shared<asio::steady_timer>(GetIOContext(), std::chrono::milliseconds(300));
	auto it = _fileDiagnosticTask.find(uri);
	if (it != _fileDiagnosticTask.end())
	{
		it->second->cancel();
		it->second = task;
	}
	else
	{
		_fileDiagnosticTask.insert({stringUri, task});
	}

	task->async_wait([this, stringUri](const asio::error_code& code)
	{
		if (code == asio::error::operation_aborted)
		{
			return;
		}
		DiagnosticFile(stringUri);
		auto it = _fileDiagnosticTask.find(stringUri);
		if (it != _fileDiagnosticTask.end())
		{
			_fileDiagnosticTask.erase(it);
		}
	});
}

std::shared_ptr<LuaParser> LanguageClient::GetFileParser(std::string_view uri)
{
	auto filename = url::UrlToFilePath(uri);
	auto it = _fileMap.find(filename);
	if (it != _fileMap.end())
	{
		return it->second->GetLuaParser();
	}

	return nullptr;
}

int LanguageClient::Run()
{
	if (_session)
	{
		int ret = _session->Run(_ioc);
		_session = nullptr;
		return ret;
	}
	return 1;
}

std::shared_ptr<LuaCodeStyleOptions> LanguageClient::GetOptions(std::string_view uriOrFilename)
{
	std::string filename;
	if (uriOrFilename.starts_with("file"))
	{
		filename = url::UrlToFilePath(uriOrFilename);
	}
	else
	{
		filename = std::string(uriOrFilename);
	}

	std::size_t matchLength = 0;
	std::shared_ptr<LuaCodeStyleOptions> options = _defaultOptions;
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
	{
		if (filename.starts_with(it->first) && it->first.size() > matchLength)
		{
			matchLength = it->first.size();
			options = it->second->Generate(filename);
		}
	}

	return options;
}

void LanguageClient::UpdateCodeStyleOptions(std::string_view workspaceUri, std::string_view configPath)
{
	auto workspace = url::UrlToFilePath(workspaceUri);
	for (auto& pair : _editorConfigVector)
	{
		if (pair.first == workspace)
		{
			pair.second = LuaEditorConfig::LoadFromFile(std::string(configPath));
			pair.second->SetWorkspace(workspace);
			pair.second->SetRootWorkspace(_root);
			return;
		}
	}

	_editorConfigVector.push_back({
		workspace,
		LuaEditorConfig::LoadFromFile(std::string(configPath))
	});
	_editorConfigVector.back().second->SetWorkspace(workspace);
	_editorConfigVector.back().second->SetRootWorkspace(_root);
}

void LanguageClient::RemoveCodeStyleOptions(std::string_view workspaceUri)
{
	auto workspace = url::UrlToFilePath(workspaceUri);
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
	{
		if (it->first == workspace)
		{
			_editorConfigVector.erase(it);
			break;
		}
	}
}

void LanguageClient::UpdateAllDiagnosis()
{
	for (auto it : _fileMap)
	{
		auto uri = url::FilePathToUrl(it.first);
		DiagnosticFile(uri);
	}
}

void LanguageClient::UpdateModuleInfo()
{
	FileFinder finder(_root);

	finder.AddIgnoreDirectory(".git");
	finder.AddIgnoreDirectory(".github");
	finder.AddIgnoreDirectory(".svn");
	finder.AddIgnoreDirectory(".idea");
	finder.AddIgnoreDirectory(".vs");
	finder.AddIgnoreDirectory(".vscode");

	finder.AddFindExtension(".lua");
	finder.AddFindExtension(".lua.txt");

	GetService<ModuleService>()->GetIndex().RebuildIndex(finder.FindFiles());
}

void LanguageClient::SetRoot(std::string_view root)
{
	_root = root;
}

asio::io_context& LanguageClient::GetIOContext()
{
	return _ioc;
}

vscode::VscodeSettings& LanguageClient::GetSettings()
{
	return _vscodeSettings;
}

void LanguageClient::SetVscodeSettings(vscode::VscodeSettings& settings)
{
	_vscodeSettings = settings;

	GetService<CodeFormatService>()->SetCustomDictionary(_vscodeSettings.spellDict);
}

uint64_t LanguageClient::GetRequestId()
{
	return ++_idCounter;
}
