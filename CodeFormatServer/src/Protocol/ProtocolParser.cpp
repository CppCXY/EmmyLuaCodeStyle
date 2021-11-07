#include "CodeFormatServer/Protocol/ProtocolParser.h"
#include <nlohmann/json.hpp>
ProtocolParser::ProtocolParser()
	: _id(0),
	  _method(),
	  _request(nullptr)
{
}

void ProtocolParser::Parse(std::string_view msg)
{
	auto jsonMessage = nlohmann::json::parse(msg);

	_id = jsonMessage["id"].get<int>();
	_method = jsonMessage["method"].get<std::string>();
	auto param = jsonMessage["param"];


}

std::shared_ptr<JsonRequestProtocol> ProtocolParser::GetRequest()
{
}

std::string_view ProtocolParser::GetMethod()
{
	return _method;
}

std::string ProtocolParser::SerializeProtocol(std::shared_ptr<JsonResponseProtocol> response)
{
	return "";
}
