#include "CodeFormatServer/LanguageService.h"

using namespace std::placeholders;

LanguageService::LanguageService()
{
}

LanguageService::~LanguageService()
{
}

bool LanguageService::Initialize()
{
#define DynamicBind(Method, ProtocolClass)\
	[this](auto request){ return Method(std::dynamic_pointer_cast<ProtocolClass>(request)); }

	_handles["initialize"] = DynamicBind(OnInitialize, InitializeRequestProtocol);
	

	return true;
}

std::shared_ptr<JsonResponseProtocol> LanguageService::Dispatch(std::string_view method,
	std::shared_ptr<JsonRequestProtocol> request)
{
	auto it = _handles.find(method);
	if(it != _handles.end())
	{
		return it->second(request);
	}
	return nullptr;
}


std::shared_ptr<JsonResponseProtocol> LanguageService::OnInitialize(std::shared_ptr<InitializeRequestProtocol> request)
{
	return nullptr;
}
