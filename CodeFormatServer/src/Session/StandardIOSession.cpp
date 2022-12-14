#include "StandardIOSession.h"

#include <iostream>
#include <asio.hpp>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <thread>

#include "Protocol/ProtocolParser.h"
#include "Protocol/ProtocolBuffer.h"
#include "LanguageServer.h"

class StandardIO {
public:
    static StandardIO &GetInstance();

    StandardIO();

    std::size_t ReadSome(char *buffer, std::size_t maxSize);

    bool HasError();

    void Write(std::string_view content);

private:
#ifndef _WIN32
    asio::io_context _ioc;
    asio::error_code _code;
    std::shared_ptr<asio::posix::stream_descriptor> _in;
    std::shared_ptr<asio::posix::stream_descriptor> _out;
#endif
};

StandardIO &StandardIO::GetInstance() {
    static StandardIO instance;
    return instance;
}

#ifndef _WIN32
StandardIO::StandardIO()
    :_ioc(1)
{
    _in = std::make_shared<asio::posix::stream_descriptor>(_ioc, STDIN_FILENO);
    _out = std::make_shared<asio::posix::stream_descriptor>(_ioc, STDOUT_FILENO);
}

std::size_t StandardIO::ReadSome(char* buffer, std::size_t maxSize)
{
    return _in->read_some(asio::buffer(buffer, maxSize), _code);
}

bool StandardIO::HasError()
{
    return _code == asio::error::eof || _code;
}

void StandardIO::Write(std::string_view content)
{
    asio::write(*_out, asio::buffer(content.data(), content.size()));
}

#else

StandardIO::StandardIO() {
}

std::size_t StandardIO::ReadSome(char *buffer, std::size_t maxSize) {
    std::cin.peek();
    return static_cast<std::size_t>(std::cin.readsome(buffer, maxSize));
}

bool StandardIO::HasError() {
    return !std::cin;
}

void StandardIO::Write(std::string_view content) {
    std::cout.write(content.data(), content.size());
    std::cout.flush();
}

#endif


int StandardIOSession::Run(LanguageServer &server) {
    auto &ioc = server.GetIOContext();
    IOSession::Run(server);
    while (true) {
        do {
            char *writableCursor = _protocolBuffer.GetWritableCursor();
            std::size_t capacity = _protocolBuffer.GetRestCapacity();

            std::size_t readSize = StandardIO::GetInstance().ReadSome(writableCursor, capacity);

            if (StandardIO::GetInstance().HasError()) {
                goto endLoop;
            }

            _protocolBuffer.SetWriteSize(readSize);

            if (_protocolBuffer.CanReadOneProtocol()) {
                break;
            }

            _protocolBuffer.FitCapacity();
        } while (true);

        do {
            auto content = _protocolBuffer.ReadOneProtocol();
            auto parser = std::make_shared<ProtocolParser>();
            parser->Parse(content);
            _protocolBuffer.Reset();
            asio::post(ioc, [this, parser, &server]() {
                std::string result = Handle(server, parser);

                if (!result.empty()) {
                    Send(result);
                }
            });
        } while (_protocolBuffer.CanReadOneProtocol());
    }
    endLoop:
    return 0;
}

void StandardIOSession::Send(std::string_view content) {
    StandardIO::GetInstance().Write(content);
}
