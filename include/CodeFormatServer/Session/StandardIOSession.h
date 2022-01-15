#pragma once

#include "IOSession.h"


class StandardIOSession: public IOSession
{
public:

	int Run(asio::io_context& ioc) override;
	void Send(std::string_view content) override;
};
