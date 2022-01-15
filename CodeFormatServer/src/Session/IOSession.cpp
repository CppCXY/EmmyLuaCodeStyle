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
}

int IOSession::Run(asio::io_context& ioc)
{
	std::thread t([&ioc]()
	{
		while(true)
		{
			ioc.run();
			std::this_thread::yield();
		}
	});
	t.detach();
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
			if (result)
			{
				return parser->SerializeProtocol(result);
			}
			std::cerr << format("request {}, it cost: {}ms\n", parser->GetMethod(),
			                    chrono::duration_cast<chrono::milliseconds>(
				                    chrono::system_clock::now() - start
			                    ).count());
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return "";
}
