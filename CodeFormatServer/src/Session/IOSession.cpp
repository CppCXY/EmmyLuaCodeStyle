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

	_service.Dispatch(parser.GetRequestMethod() , parser.GetRequest());

	return "";
}
