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

	auto param = parser.GetParam();

	if (param)
	{
		auto result = _service.Dispatch(parser.GetMethod(), param);
		if (result) {
			return parser.SerializeProtocol(result);
		}
	}

	return "";
}
