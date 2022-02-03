#pragma once
#include <string>
#include <thread>
#include "asio/io_context.hpp"
#include "CodeFormatServer/LanguageService.h"
#include "CodeFormatServer/Protocol/ProtocolBuffer.h"
#include "CodeFormatServer/Protocol/ProtocolParser.h"

class IOSession
{
public:
	IOSession();
	virtual ~IOSession();

	virtual int Run(asio::io_context& ioc);
	virtual void Send(std::string_view content) = 0;
protected:
	std::string Handle(std::shared_ptr<ProtocolParser> parser);
	ProtocolBuffer _protocolBuffer;
private:
	LanguageService _service;
#ifndef __APPLE__
	std::shared_ptr<std::jthread> _logicThread;
#else
	std::shared_ptr<std::thread> _logicThread = nullptr;
	volatile bool _running = true;
#endif

};
