#pragma once

#include <memory>
#include <functional>
#include <map>
#include "Protocol/JsonResponseProtocol.h"
#include "Protocol/Request/JsonRequestProtocol.h"
#include "Protocol/Request/InitialRequestProtocol.h"

class LanguageService
{
public:
	using MessageHandle = std::function<std::shared_ptr<JsonResponseProtocol>(std::shared_ptr<JsonRequestProtocol>)>;

	LanguageService();
	~LanguageService();

	bool Initialize();

	std::shared_ptr<JsonResponseProtocol> Dispatch(std::string_view method,
	                                               std::shared_ptr<JsonRequestProtocol> request);

private:
	std::shared_ptr<JsonResponseProtocol> OnInitialize(std::shared_ptr<InitializeRequestProtocol> request);

	// std::shared_ptr<JsonResponseProtocol> OnDocumentFormatting(std::shared_ptr<FormatRequestProtocol> request);

	std::map<std::string, MessageHandle, std::less<>> _handles;
};
