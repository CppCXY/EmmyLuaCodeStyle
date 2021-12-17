#pragma once

#include "IOSession.h"


class StandardIOSession: public IOSession
{
public:

	void Run() override;
	void Send(std::string_view content) override;
};
