#pragma once

#include <cstdlib>
#include <string_view>
#include <memory>
#include "CodeFormatServer/Protocol/Request/JsonRequestProtocol.h"
#include "CodeFormatServer/Protocol/JsonResponseProtocol.h"

class ProtocolParser
{
public:
	ProtocolParser();

	void Parse(std::string_view msg);

	std::shared_ptr<JsonRequestProtocol> GetRequest();

	std::string_view GetMethod();

	std::string SerializeProtocol(std::shared_ptr<JsonResponseProtocol> response);
private:
	int _id;
	std::string _method;
	std::shared_ptr<JsonRequestProtocol> _request;
};