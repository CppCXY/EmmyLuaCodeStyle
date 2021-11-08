#include "CodeFormatServer/Session/IOSession.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include "CodeFormatServer/Protocol/ProtocolParser.h"

IOSession::IOSession()
{
	_service.Initialize();
}

IOSession::~IOSession()
{
}

std::string IOSession::Handle(std::string_view msg)
{
	ProtocolParser parser;

	parser.Parse(msg);

	auto request = parser.GetRequest();

	if (request)
	{
		auto response = _service.Dispatch(parser.GetMethod(), request);
		if (response) {
			return parser.SerializeProtocol(response);
		}
	}

	return "";
}
