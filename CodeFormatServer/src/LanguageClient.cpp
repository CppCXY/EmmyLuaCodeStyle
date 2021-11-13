#include "CodeFormatServer/LanguageClient.h"

#include "CodeService/LuaFormatter.h"
#include "LuaParser/LuaParser.h"
#include "Util/format.h"

LanguageClient& LanguageClient::GetInstance()
{
	static LanguageClient instance;
	return instance;
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

void LanguageClient::CacheFile(std::string_view uri, std::string text)
{
	_fileMap[std::string(uri)] = std::move(text);
}

void LanguageClient::ReleaseFile(std::string_view uri)
{
	auto it = _fileMap.find(uri);
	if (it != _fileMap.end())
	{
		_fileMap.erase(it);
	}
}

void LanguageClient::DiagnosticFile(std::string_view uri)
{
	auto it = _fileMap.find(uri);
	if (it == _fileMap.end())
	{
		return;
	}

	std::string text = it->second;

	std::shared_ptr<LuaParser> parser = LuaParser::LoadFromBuffer(std::move(text));

	if(parser->HasError())
	{
		return;
	}

	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, _options);
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

std::string LanguageClient::GetFile(std::string_view uri)
{
	auto it = _fileMap.find(uri);
	if(it != _fileMap.end())
	{
		return it->second;
	}

	return "";
}

void LanguageClient::Run()
{
	if (_session)
	{
		_session->Run();
	}
}

LuaFormatOptions& LanguageClient::GetOptions()
{
	return _options;
}
