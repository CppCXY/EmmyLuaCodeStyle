#pragma once

#include <memory>
#include "Session/IOSession.h"
#include "CodeService/LuaCodeStyleOptions.h"
#include "CodeService/LuaEditorConfig.h"
#include "LuaParser/LuaParser.h"
#include "Service/ServiceType.h"
#include "VirtualFile/VirtualFile.h"

// 前置声明
class Service;

template <class Derived>
concept ServiceClass = requires(Derived d)
{
	std::is_base_of<Service, Derived>::value;
	Derived::ServiceIndex;
};

class LanguageClient: public std::enable_shared_from_this<LanguageClient>
{
public:
	static LanguageClient& GetInstance();

	LanguageClient();

	void InitializeService();

	void SetSession(std::shared_ptr<IOSession> session);

	std::shared_ptr<IOSession> GetSession();

	void SendNotification(std::string_view method, std::shared_ptr<vscode::Serializable> param);

	void SendRequest(std::string_view method, std::shared_ptr<vscode::Serializable> param);

	void UpdateFile(std::string_view uri, vscode::Range range, std::string&& text);

	void ReleaseFile(std::string_view uri);

	void DiagnosticFile(std::string_view uri);

	std::shared_ptr<LuaParser> GetFileParser(std::string_view uri);

	void Run();

	std::shared_ptr<LuaCodeStyleOptions> GetOptions(std::string_view uriOrFilename);

	void UpdateCodeStyleOptions(std::string_view workspaceUri, std::string_view configPath);

	void RemoveCodeStyleOptions(std::string_view workspaceUri);

	void UpdateAllDiagnosis();

	void UpdateModuleInfo();

	void SetRoot(std::string_view root);

	template <ServiceClass Service>
	std::shared_ptr<Service> GetService();

private:
	uint64_t GetRequestId();

	template <ServiceClass Service, typename... ARGS>
	std::shared_ptr<Service> AddService(ARGS ...args);

	std::shared_ptr<IOSession> _session;
	// filePath 到 file的映射
	std::map<std::string, std::shared_ptr<VirtualFile>, std::less<>> _fileMap;

	std::vector<std::pair<std::string, std::shared_ptr<LuaEditorConfig>>> _editorConfigVector;

	std::shared_ptr<LuaCodeStyleOptions> _defaultOptions;

	uint64_t _idCounter;

	std::string _root;

	std::array<std::shared_ptr<Service>, static_cast<std::size_t>(ServiceType::ServiceCount)> _services;
};

template <ServiceClass Service>
std::shared_ptr<Service> LanguageClient::GetService()
{
	return std::dynamic_pointer_cast<Service>(
		_services[static_cast<std::size_t>(Service::ServiceIndex)]);
}

template <ServiceClass Service, typename ... ARGS>
std::shared_ptr<Service> LanguageClient::AddService(ARGS... args)
{
	auto index = Service::ServiceIndex;
	auto service = std::make_shared<Service>(shared_from_this(), std::forward<ARGS>(args)...);
	_services[static_cast<std::size_t>(index)] = service;
	return service;
}
