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
	while (true)
	{
		do
		{
			asio::error_code code;

			char* writableCursor = _protocolBuffer.GetWritableCursor();
			std::size_t capacity = _protocolBuffer.GetRestCapacity();

			std::size_t readSize = _socket.read_some(asio::buffer(writableCursor, capacity), code);
			if (code == asio::error::eof || code)
			{
				goto endLoop;
			}

			_protocolBuffer.SetWriteSize(readSize);

			if (_protocolBuffer.CanReadOneProtocol())
			{
				break;
			}

			_protocolBuffer.FitCapacity();
		}
		while (true);

		do {
			auto content = _protocolBuffer.ReadOneProtocol();
			// std::cout << content << std::endl;
			std::string result = Handle(content);

			_protocolBuffer.Reset();
			if (!result.empty()) {
				Send(result);
			}
		} while (_protocolBuffer.CanReadOneProtocol());
	}
endLoop:
	return;
}

void SocketIOSession::Send(std::string_view content)
{
	asio::write(_socket, asio::buffer(content));
}
