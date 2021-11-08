#pragma once
#include <nlohmann/json_fwd.hpp>

class JsonRequestProtocol
{
public:
	JsonRequestProtocol();

	virtual ~JsonRequestProtocol();
	virtual void Initialize(nlohmann::json param) = 0;
};

