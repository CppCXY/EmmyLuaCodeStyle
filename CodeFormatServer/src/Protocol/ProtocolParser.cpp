#include "CodeFormatServer/Protocol/ProtocolParser.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "CodeFormatServer/LanguageService.h"
#include "CodeFormatServer/Protocol/Request/RequestProtocolFactory.h"

ProtocolParser::ProtocolParser()
	: _id(0),
	  _method(),
	  _request(nullptr)
{
}

void ProtocolParser::Parse(std::string_view msg)
{
	std::cout << msg << std::endl;
	auto jsonMessage = nlohmann::json::parse(msg);

	_id = jsonMessage["id"].get<int>();
	_method = jsonMessage["method"].get<std::string>();
	auto params = jsonMessage["params"];

	_request = RequestProtocolFactory::GetInstance().MakeProtocol(_method, params);
}

std::shared_ptr<JsonRequestProtocol> ProtocolParser::GetRequest()
{
	return _request;
}

std::string_view ProtocolParser::GetMethod()
{
	return _method;
}

std::string ProtocolParser::SerializeProtocol(std::shared_ptr<JsonResponseProtocol> response)
{
	return "";
}
