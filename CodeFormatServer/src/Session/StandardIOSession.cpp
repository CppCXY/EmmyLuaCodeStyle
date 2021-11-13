#include "CodeFormatServer/Session/StandardIOSession.h"

#include <iostream>
#include <thread>
#include "CodeFormatServer/Protocol/ProtocolParser.h"
#include "CodeFormatServer/Protocol/ProtocolBuffer.h"

void StandardIOSession::Run()
{
	ProtocolBuffer protocolBuffer(1024);
	while (true)
	{
		std::size_t readSize = 0;
		do
		{
			char* writableCursor = protocolBuffer.GetWritableCursor();
			std::size_t capacity = protocolBuffer.GetRestCapacity();

			std::cin.peek();
			readSize += std::cin.readsome(writableCursor, capacity);
			if (!std::cin)
			{
				goto endLoop;
			}

			protocolBuffer.SetReadableSize(readSize);

			if (protocolBuffer.CanReadOneProtocol())
			{
				break;
			}

		} while (true);

		do {
			std::string result = Handle(protocolBuffer.ReadOneProtocol());

			protocolBuffer.Reset();
			if (!result.empty()) {
				Send(result);
			}
		} while (protocolBuffer.CanReadOneProtocol());
	}
endLoop:
	return;
}

void StandardIOSession::Send(std::string_view content)
{
	std::cout.write(content.data(), content.size());
}
