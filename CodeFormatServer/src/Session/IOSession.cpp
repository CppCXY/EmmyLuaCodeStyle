#include "CodeFormatServer/Session/IOSession.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include "CodeFormatServer/Protocol/ProtocolParser.h"

IOSession::IOSession()
	: _protocolBuffer(65535)
{
	_service.Initialize();
}

IOSession::~IOSession()
{
}

void IOSession::Run()
{
}

std::string IOSession::Handle(std::string_view msg)
{
	try
	{
		ProtocolParser parser;

		parser.Parse(msg);

		auto param = parser.GetParam();

		if (param)
		{
			auto result = _service.Dispatch(parser.GetMethod(), param);
			if (result)
			{
				return parser.SerializeProtocol(result);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return "";
}
