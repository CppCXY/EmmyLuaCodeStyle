#pragma once

#include <functional>
#include "JsonRequestProtocol.h"

class RequestProtocolFactory
{
public:
	using MakeHandle = std::function<std::shared_ptr<JsonRequestProtocol>(nlohmann::json)>;

	static RequestProtocolFactory& GetInstance();

	RequestProtocolFactory();
	~RequestProtocolFactory();

	std::shared_ptr<JsonRequestProtocol> MakeProtocol(std::string_view method, nlohmann::json param);

	void Initialize();

private:
	std::map<std::string, MakeHandle, std::less<>> _handle;
};
