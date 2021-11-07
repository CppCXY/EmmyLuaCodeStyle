#pragma once
#include <nlohmann/json_fwd.hpp>

class JsonRequestProtocol
{
public:
	JsonRequestProtocol(nlohmann::json param);

	virtual ~JsonRequestProtocol();
	virtual void Initialize();
};

