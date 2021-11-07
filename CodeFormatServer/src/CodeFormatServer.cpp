#include <iostream>
#include <asio.hpp>
#include "CodeFormatServer/LanguageClient.h"
#include "CodeFormatServer/Session/SocketIOSession.h"
#include "CodeFormatServer/Session/StandardIOSession.h"

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
        // LanguageClient::GetInstance().SetSession(std::make_shared<StandardIOSession>());
        // LanguageClient::GetInstance().Run();
    }

   

    return 0;
}