#pragma once

#include <memory>
#include "Session/IOSession.h"

class LanguageClient
{
public:
	static LanguageClient& GetInstance();

	void SetSession(std::shared_ptr<IOSession> session);

	std::shared_ptr<IOSession> GetSession();

	void SendNotification(std::string_view method, std::shared_ptr<vscode::Serializable> param);

	void Run();
private:
	std::shared_ptr<IOSession> _session;
};