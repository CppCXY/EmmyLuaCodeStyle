#include "CodeFormatServer/Session/SocketIOSession.h"

#include <iostream>

#include "CodeFormatServer/Protocol/ProtocolParser.h"
#include "CodeFormatServer/Protocol/ProtocolBuffer.h"
using namespace asio::ip;

SocketIOSession::SocketIOSession(asio::ip::tcp::socket&& socket)
	: _socket(std::move(socket))
{
}

void SocketIOSession::Run()
{
	ProtocolBuffer protocolBuffer(1024);
	while (true)
	{
		asio::error_code code;
		std::size_t readSize = 0;
		do
		{
			char* writableCursor = protocolBuffer.GetWritableCursor();
			std::size_t capacity = protocolBuffer.GetRestCapacity();

			readSize += _socket.read_some(asio::buffer(writableCursor, capacity), code);
			if (code == asio::error::eof || code)
			{
				goto endLoop;
			}

			protocolBuffer.SetReadableSize(readSize);

			if (protocolBuffer.CanReadOneProtocol())
			{
				break;
			}
		}
		while (true);

		std::string result = Handle(protocolBuffer.ReadOneProtocol());

		protocolBuffer.Reset();
		if (!result.empty()) {
			asio::write(_socket, asio::buffer(result));
		}
	}
endLoop:
	return;
}
