#include "CodeFormatServer/LanguageClient.h"

LanguageClient& LanguageClient::GetInstance()
{
	static LanguageClient instance;
	return instance;
}

void LanguageClient::SetSession(std::shared_ptr<IOSession> session)
{
	_session = session;
}

void LanguageClient::Run()
{
	if (_session)
	{
		_session->Run();
	}
}
