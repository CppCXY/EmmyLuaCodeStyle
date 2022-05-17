#include "CodeFormatServer/Session/IOSession.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include "CodeFormatServer/Protocol/ProtocolParser.h"
#include <chrono>
#include <thread>
#include "Util/format.h"

namespace chrono = std::chrono;

IOSession::IOSession()
	: _protocolBuffer(65535)
{
	_service.Initialize();
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

int IOSession::Run(asio::io_context& ioc)
{
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

std::string IOSession::Handle(std::shared_ptr<ProtocolParser> parser)
{
	try
	{
		auto params = parser->GetParams();

		if (!params.is_null())
		{
			auto start = chrono::system_clock::now();
			auto result = _service.Dispatch(parser->GetMethod(), params);
			std::cerr << Util::format("request {}, it cost: {}ms\n", parser->GetMethod(),
			                    chrono::duration_cast<chrono::milliseconds>(
				                    chrono::system_clock::now() - start
			                    ).count());
			if (result)
			{
				return parser->SerializeProtocol(result);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return "";
}
