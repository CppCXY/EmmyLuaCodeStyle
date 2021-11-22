#pragma once
#include <string>
#include "CodeFormatServer/LanguageService.h"
#include "CodeFormatServer/Protocol/ProtocolBuffer.h"

class IOSession
{
public:
	IOSession();
	virtual ~IOSession();

	virtual void Run();
	virtual void Send(std::string_view content) = 0;
protected:
	std::string Handle(std::string_view msg);
	ProtocolBuffer _protocolBuffer;
private:
	LanguageService _service;
};
