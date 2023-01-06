#include "IOSession.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include "Protocol/ProtocolParser.h"
#include <chrono>
#include <thread>
#include "Util/format.h"
#include "LanguageServer.h"

namespace chrono = std::chrono;

IOSession::IOSession()
	: _protocolBuffer(65535)
{
}

IOSession::~IOSession()
{
	if(_logicThread)
	{
#ifndef __APPLE__
		_logicThread->request_stop();
#else
		_running = false;
		_logicThread->join();
#endif
	}
}

int IOSession::Run(LanguageServer& server)
{
    auto& ioc = server.GetIOContext();
	//苹果暂未支持 这个jthread
#ifndef __APPLE__
	_logicThread = std::make_shared<std::jthread>([&ioc](std::stop_token st)
	{
		while (!st.stop_requested())
		{
			ioc.run();
			ioc.reset();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});
#else
	_logicThread = std::make_shared<std::thread>([&ioc, this]()
    {
        while (_running)
        {
            ioc.run();
            ioc.reset();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
#endif

	return 0;
}

std::string IOSession::Handle(LanguageServer& server, std::shared_ptr<ProtocolParser> parser)
{
#if !defined(_DEBUG)
	try
#endif
	{
        auto& lspHandle = server.GetLSPHandle();
		auto params = parser->GetParams();

		if (!params.is_null())
		{
			auto result = lspHandle.Dispatch(parser->GetMethod(), params);
			if (result)
			{
				return parser->SerializeProtocol(result);
			}
		}
	}
#if !defined(_DEBUG)
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
#endif
	return "";
}
