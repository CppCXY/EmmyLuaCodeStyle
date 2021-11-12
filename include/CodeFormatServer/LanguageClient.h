#pragma once

#include <memory>
#include "Session/IOSession.h"
#include "CodeService/LuaFormatOptions.h"

class LanguageClient
{
public:
	static LanguageClient& GetInstance();

	void SetSession(std::shared_ptr<IOSession> session);

	std::shared_ptr<IOSession> GetSession();

	void SendNotification(std::string_view method, std::shared_ptr<vscode::Serializable> param);

	void CacheFile(const std::string& uri, std::string text);

	void DiagnosticFile(const std::string& uri);

	std::string GetFile(const std::string& uri);

	void Run();

	LuaFormatOptions& GetOptions();
private:
	std::shared_ptr<IOSession> _session;
	// uri µ½file textµÄÓ³Éä
	std::map<std::string, std::string, std::less<>> _fileMap;

	LuaFormatOptions _options;
};