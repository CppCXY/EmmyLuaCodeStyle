#pragma once
#include "JsonRequestProtocol.h"

class InitializeRequestProtocol : public JsonRequestProtocol
{
public:
	InitializeRequestProtocol();
	void Initialize(nlohmann::json param) override;
};

