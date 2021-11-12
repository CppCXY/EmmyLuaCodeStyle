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

std::shared_ptr<IOSession> LanguageClient::GetSession()
{
	return _session;
}

void LanguageClient::Run()
{
	if (_session)
	{
		_session->Run();
	}
}
