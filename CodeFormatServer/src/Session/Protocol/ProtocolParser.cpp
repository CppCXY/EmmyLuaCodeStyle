#include "ProtocolParser.h"
#include "nlohmann/json.hpp"
#include "Util/format.h"
#include "LSP/LSPHandle.h"

ProtocolParser::ProtocolParser()
	: _id(nullptr)
{
}

void ProtocolParser::Parse(std::string_view msg)
{
	auto jsonMessage = nlohmann::json::parse(msg);
	if (jsonMessage["id"].is_number())
	{
		_id = jsonMessage["id"].get<int>();
	}
	else if (jsonMessage["id"].is_string())
	{
		_id = jsonMessage["id"].get<std::string>();
	}

	if (jsonMessage["method"].is_string())
	{
		_method = jsonMessage["method"].get<std::string>();
	}
	if (jsonMessage["params"].is_object())
	{
		_params = jsonMessage["params"];
	}
}

nlohmann::json ProtocolParser::GetParams()
{
	return _params;
}

std::string_view ProtocolParser::GetMethod()
{
	return _method;
}

std::string ProtocolParser::SerializeProtocol(std::shared_ptr<lsp::Serializable> result)
{
	nlohmann::json json;
	if (_id.index() == 0)
	{
		json["id"] = std::get<int>(_id);
	}
	else if (_id.index() == 1)
	{
		json["id"] = std::get<std::string>(_id);
	}

	json["result"] = result->Serialize();
	json["jsonrpc"] = "2.0";
	auto dumpResult = json.dump(-1,' ', true, nlohmann::detail::error_handler_t::ignore);
	std::string message = util::format("Content-Length:{}\r\n\r\n", dumpResult.size());

	message.append(dumpResult);
	return std::move(message);
}
