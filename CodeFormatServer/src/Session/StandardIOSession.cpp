#include "CodeFormatServer/Session/StandardIOSession.h"

#include <iostream>
#include <thread>
#include "CodeFormatServer/Protocol/ProtocolParser.h"
#include "CodeFormatServer/Protocol/ProtocolBuffer.h"

void StandardIOSession::Run()
{

	while (true)
	{
		do
		{
			char* writableCursor = _protocolBuffer.GetWritableCursor();
			std::size_t capacity = _protocolBuffer.GetRestCapacity();

			std::cin.peek();
			std::size_t readSize = std::cin.readsome(writableCursor, capacity);
			if (!std::cin)
			{
				goto endLoop;
			}

			_protocolBuffer.WriteBuff(readSize);

			if (_protocolBuffer.CanReadOneProtocol())
			{
				break;
			}

		} while (true);

		do {
			std::string result = Handle(_protocolBuffer.ReadOneProtocol());

			_protocolBuffer.Reset();
			if (!result.empty()) {
				Send(result);
			}
		} while (_protocolBuffer.CanReadOneProtocol());
	}
endLoop:
	return;
}

void StandardIOSession::Send(std::string_view content)
{
	std::cout.write(content.data(), content.size());
}
