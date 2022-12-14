#pragma once

#include "IOSession.h"


class StandardIOSession: public IOSession
{
public:

	int Run(LanguageServer& server) override;
	void Send(std::string_view content) override;
};
