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

}

void RequestProtocolFactory::Initialize()
{
#define DynamicBind(ProtocolClass) \
	[] (auto param)->std::shared_ptr<JsonRequestProtocol> { \
	return std::make_shared<ProtocolClass>(param);\
	}

	_handle["initialize"] = DynamicBind(InitializeRequestProtocol);
}
