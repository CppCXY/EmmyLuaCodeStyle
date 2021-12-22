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

		auto params = parser.GetParams();

		if (!params.is_null())
		{
			auto result = _service.Dispatch(parser.GetMethod(), params);
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
