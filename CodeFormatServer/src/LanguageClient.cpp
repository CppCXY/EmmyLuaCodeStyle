#include "CodeFormatServer/LanguageClient.h"

#include "CodeService/LuaEditorConfig.h"
#include "CodeService/LuaFormatter.h"
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "Util/Url.h"
#include "Util/FileFinder.h"

LanguageClient& LanguageClient::GetInstance()
{
	static LanguageClient instance;
	return instance;
}

LanguageClient::LanguageClient()
	: _defaultOptions(std::make_shared<LuaCodeStyleOptions>()),
	  _idCounter(0),
	  _moduleIndex(std::make_shared<ModuleIndex>())
{
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
		std::string message = format("Content-Length:{}\r\n\r\n", dumpResult.size());

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
		std::string message = format("Content-Length:{}\r\n\r\n", dumpResult.size());

		message.append(dumpResult);
		_session->Send(std::move(message));
	}
}

void LanguageClient::CacheFile(std::string_view uri, std::string&& text)
{
	auto filename = url::UrlToFilePath(uri);
	auto virtualFile = std::make_shared<VirtualFile>(filename, std::move(text));
	_fileMap[filename] = virtualFile;
}

void LanguageClient::ReleaseFile(std::string_view uri)
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

	auto options = GetOptions(uri);
	if (!options->enable_check_codestyle)
	{
		auto vscodeDiagnosis = std::make_shared<vscode::PublishDiagnosticsParams>();
		vscodeDiagnosis->uri = uri;
		SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
		return;
	}

	std::shared_ptr<LuaParser> parser = it->second->GetLuaParser();

	if (parser->HasError())
	{
		return;
	}

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto diagnosisInfos = formatter.GetDiagnosisInfos();

	auto vscodeDiagnosis = std::make_shared<vscode::PublishDiagnosticsParams>();
	vscodeDiagnosis->uri = uri;

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




	SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
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

void LanguageClient::Run()
{
	if (_session)
	{
		_session->Run();
	}
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

void LanguageClient::UpdateOptions(std::string_view workspaceUri, std::string_view configPath)
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

void LanguageClient::RemoveOptions(std::string_view workspaceUri)
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
	FileFinder finder(_root);

	finder.AddIgnoreDirectory(".git");
	finder.AddIgnoreDirectory(".github");
	finder.AddIgnoreDirectory(".svn");
	finder.AddIgnoreDirectory(".idea");
	finder.AddIgnoreDirectory(".vs");
	finder.AddIgnoreDirectory(".vscode");

	finder.AddFindExtension(".lua");

	_moduleIndex->ReBuildIndex(finder.FindFiles());

	for (auto it : _fileMap)
	{
		auto uri = url::FilePathToUrl(it.first);
		DiagnosticFile(uri);
	}
}

void LanguageClient::SetRoot(std::string_view root)
{
	_root = root;
}

uint64_t LanguageClient::GetRequestId()
{
	return ++_idCounter;
}
