#include "SocketIOSession.h"
#include <iostream>
#include "Protocol/ProtocolParser.h"
#include "Protocol/ProtocolBuffer.h"
#include "LanguageServer.h"

using namespace asio::ip;

SocketIOSession::SocketIOSession(asio::ip::tcp::socket&& socket)
	: _socket(std::move(socket))
{
}

int SocketIOSession::Run(LanguageServer& server)
{
    auto &ioc = server.GetIOContext();
	IOSession::Run(server);
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

		do
		{
			auto content = _protocolBuffer.ReadOneProtocol();
			auto parser = std::make_shared<ProtocolParser>();
			parser->Parse(content);
			_protocolBuffer.Reset();
			asio::post(ioc, [this, parser, &server]()
			{
				std::string result = Handle(server, parser);

				if (!result.empty())
				{
					Send(result);
				}
			});
		}
		while (_protocolBuffer.CanReadOneProtocol());
	}
endLoop:
	return 0;
}

void SocketIOSession::Send(std::string_view content)
{
	asio::write(_socket, asio::buffer(content));
}
