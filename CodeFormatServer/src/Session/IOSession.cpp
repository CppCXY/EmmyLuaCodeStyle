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
		_logicThread->request_stop();
	}
}

int IOSession::Run(asio::io_context& ioc)
{
	_logicThread = std::make_shared<std::jthread>([&ioc](std::stop_token st)
	{
		while (!st.stop_requested())
		{
			ioc.run();
			ioc.reset();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

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
			std::cerr << format("request {}, it cost: {}ms\n", parser->GetMethod(),
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
