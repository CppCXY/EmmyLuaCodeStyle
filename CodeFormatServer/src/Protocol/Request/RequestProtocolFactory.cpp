#include "CodeFormatServer/Protocol/Request/RequestProtocolFactory.h"
#include "CodeFormatServer/Protocol/Request/InitialRequestProtocol.h"
#include <nlohmann/json.hpp>

RequestProtocolFactory& RequestProtocolFactory::GetInstance()
{
	static RequestProtocolFactory instance;
	return instance;
}

RequestProtocolFactory::RequestProtocolFactory()
{
	Initialize();
}

RequestProtocolFactory::~RequestProtocolFactory()
{
}

std::shared_ptr<JsonRequestProtocol> RequestProtocolFactory::MakeProtocol(std::string_view method, nlohmann::json param)
{
	auto it = _handle.find(method);
	if (it != _handle.end())
	{
		return it->second(param);
	}
	return nullptr;
}

void RequestProtocolFactory::Initialize()
{
#define DynamicBind(ProtocolClass) \
	[] (auto param)->std::shared_ptr<JsonRequestProtocol> { \
	auto protocol = std::make_shared<ProtocolClass>();\
	protocol->Initialize(param);\
	return protocol;\
	}

	_handle["initialize"] = DynamicBind(InitializeRequestProtocol);
}
