#include <iostream>
#include <asio.hpp>
#include "CodeFormatServer/LanguageClient.h"
#include "CodeFormatServer/Session/SocketIOSession.h"
#include "CodeFormatServer/Session/StandardIOSession.h"
#include <thread>
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

    if(argc > 1)
    {
        int port = std::stoi(argv[1]);
        asio::io_context ioc(1);

    	tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), port));
        while(true)
        {
            auto socket = acceptor.accept();
            LanguageClient::GetInstance().SetSession(std::make_shared<SocketIOSession>(std::move(socket)));
            LanguageClient::GetInstance().Run();
        }
    }
    else
    {
        SET_BINARY_MODE();
        LanguageClient::GetInstance().SetSession(std::make_shared<StandardIOSession>());
        LanguageClient::GetInstance().Run();
    }

   

    return 0;
}