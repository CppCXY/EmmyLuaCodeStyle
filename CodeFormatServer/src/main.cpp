#include <asio.hpp>
#include "LanguageServer.h"
#include "Session/SocketIOSession.h"
#include "Session/StandardIOSession.h"
// https://stackoverflow.com/questions/1598985/c-read-binary-stdin
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE() _setmode(_fileno(stdin), _O_BINARY);\
	_setmode(_fileno(stdout), _O_BINARY)
#else
# define SET_BINARY_MODE() (void)0
#endif

using namespace asio::ip;

int main(int argc, char** argv)
{
    LanguageServer server;
	auto& ioc = server.GetIOContext();
	if (argc > 1)
	{
		int port = std::stoi(argv[1]);
		tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), port));

		auto socket = acceptor.accept();
		server.SetSession(std::make_shared<SocketIOSession>(std::move(socket)));
	}
	else
	{
		SET_BINARY_MODE();
		server.SetSession(std::make_shared<StandardIOSession>());
	}

	return server.Run();
}
