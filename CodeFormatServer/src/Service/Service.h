#pragma once

#include <memory>
#include "ServiceType.h"

class LanguageServer;

class Service
{
public:
	LANGUAGE_SERVICE(Service);

	explicit Service(LanguageServer* owner);

	virtual ~Service() = default;

	virtual bool Initialize() { return true; }

	virtual void Start() {};

protected:
	LanguageServer* _owner;
};
