#pragma once

#include <memory>
#include "Session/IOSession.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeService/LuaEditorConfig.h"
#include "LuaParser/LuaParser.h"
#include "VirtualFile/VirtualFile.h"

class LanguageClient
{
public:
	static LanguageClient& GetInstance();

	LanguageClient();

	void SetSession(std::shared_ptr<IOSession> session);

	std::shared_ptr<IOSession> GetSession();

	void SendNotification(std::string_view method, std::shared_ptr<vscode::Serializable> param);

	void CacheFile(std::string_view uri, std::string&& text);

	void ReleaseFile(std::string_view uri);

	void DiagnosticFile(std::string_view uri);

	std::shared_ptr<LuaParser> GetFileParser(std::string_view uri);

	void Run();

	std::shared_ptr<LuaCodeStyleOptions> GetOptions(std::string_view uri);

	void UpdateOptions(std::string_view workspaceUri, std::string_view configPath);

	void RemoveOptions(std::string_view workspaceUri);

	void UpdateAllDiagnosis();
private:
	std::shared_ptr<IOSession> _session;
	// filePath 到 file的映射
	std::map<std::string, std::shared_ptr<VirtualFile>, std::less<>> _fileMap;

	std::vector<std::pair<std::string, std::shared_ptr<LuaEditorConfig>>> _editorConfigVector;

	std::shared_ptr<LuaCodeStyleOptions> _defaultOptions;
};
