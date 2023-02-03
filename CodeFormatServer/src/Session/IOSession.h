#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <asio/io_context.hpp>
#include "Protocol/ProtocolBuffer.h"
#include "Protocol/ProtocolParser.h"


class LanguageServer;

class IOSession
{
public:
	IOSession();
	virtual ~IOSession();

	virtual int Run(LanguageServer& server);
	virtual void Send(std::string_view content) = 0;
protected:
	std::string Handle(LanguageServer& server, std::shared_ptr<ProtocolParser> parser);
	ProtocolBuffer _protocolBuffer;
private:
#ifndef __APPLE__
	std::shared_ptr<std::jthread> _logicThread;
#else
	std::shared_ptr<std::thread> _logicThread = nullptr;
	std::atomic<bool> _running = true;
#endif

};
