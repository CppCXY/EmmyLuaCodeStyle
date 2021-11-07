#pragma once
#include "JsonRequestProtocol.h"

class InitializeRequestProtocol : public JsonRequestProtocol
{
public:
	void ParseFrom(nlohmann::json param) override;

	



};
