#include "LanguageServer.h"
#include <iterator>
#include <fstream>
#include "Util/format.h"
#include "Util/Url.h"
#include "Util/FileFinder.h"
#include "asio.hpp"
#include "Service/Service.h"
#include "Service/CodeActionService.h"
#include "Service/FormatService.h"
#include "Service/DiagnosticService.h"
#include "Service/ConfigService.h"
#include "Service/CommandService.h"

LanguageServer::LanguageServer()
        : _idCounter(0),
          _ioc(1),
          _lspHandle(this) {
}

void LanguageServer::InitializeService() {
    AddService<FormatService>();
    AddService<DiagnosticService>();
	AddService<CommandService>();
	AddService<CodeActionService>();
    AddService<ConfigService>();

    for (auto &service: _services) {
        service->Initialize();
    }

    for (auto &service: _services) {
        service->Start();
    }
}

void LanguageServer::SetSession(std::shared_ptr<IOSession> session) {
    _session = session;
}

std::shared_ptr<IOSession> LanguageServer::GetSession() {
    return _session;
}

void LanguageServer::SendNotification(std::string_view method, std::shared_ptr<lsp::Serializable> param) {
    auto json = nlohmann::json::object();
    json["jsonrpc"] = "2.0";
    json["method"] = method;
    if (param != nullptr) {
        json["params"] = param->Serialize();
    } else {
        json["params"] = nullptr;
    }

    if (_session) {
        auto dumpResult = json.dump();
        std::string message = util::format("Content-Length:{}\r\n\r\n", dumpResult.size());

        message.append(dumpResult);
        _session->Send(std::move(message));
    }
}

void LanguageServer::SendRequest(std::string_view method, std::shared_ptr<lsp::Serializable> param) {
    auto json = nlohmann::json::object();
    json["jsonrpc"] = "2.0";
    json["method"] = method;
    json["id"] = GetRequestId();
    if (param) {
        json["params"] = param->Serialize();
    } else {
        json["params"] = nullptr;
    }
    if (_session) {
        auto dumpResult = json.dump();
        std::string message = util::format("Content-Length:{}\r\n\r\n", dumpResult.size());

        message.append(dumpResult);
        _session->Send(std::move(message));
    }
}

int LanguageServer::Run() {
    if (_session) {
        int ret = _session->Run(*this);
        _session = nullptr;
        return ret;
    }
    return 1;
}

asio::io_context &LanguageServer::GetIOContext() {
    return _ioc;
}

uint64_t LanguageServer::GetRequestId()
{
	return ++_idCounter;
}

LSPHandle &LanguageServer::GetLSPHandle() {
    return _lspHandle;
}

VirtualFileSystem &LanguageServer::GetVFS() {
    return _vfs;
}
