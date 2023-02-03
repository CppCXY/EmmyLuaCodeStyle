#pragma once

#include <memory>

#include <asio.hpp>
#include <asio/io_context.hpp>

#include "Session/IOSession.h"
#include "CodeService/Config/LuaEditorConfig.h"
#include "Service/ServiceType.h"
#include "VFS/VirtualFile.h"
#include "LSP/LSPHandle.h"
#include "VFS/VirtualFileSystem.h"
#include "Service/Service.h"

template<class Derived>
concept ServiceClass = requires(Derived d)
{
    std::is_base_of<Service, Derived>::value;
    Derived::ServiceIndex;
};

class LanguageServer {
public:
    LanguageServer();

    void InitializeService();

    void SetSession(std::shared_ptr<IOSession> session);

    std::shared_ptr<IOSession> GetSession();

    void SendNotification(std::string_view method, std::shared_ptr<lsp::Serializable> params);

    void SendRequest(std::string_view method, std::shared_ptr<lsp::Serializable> params);

    int Run();

    asio::io_context &GetIOContext();

    template<ServiceClass Service, typename ... ARGS>
    void AddService(ARGS &&... args);

    template<ServiceClass Service>
    Service *GetService();

    LSPHandle &GetLSPHandle();

    VirtualFileSystem &GetVFS();

private:
    uint64_t GetRequestId();

    uint64_t _idCounter;

    std::shared_ptr<IOSession> _session;

    asio::io_context _ioc;

    std::array<std::unique_ptr<Service>, static_cast<std::size_t>(ServiceType::ServiceCount)> _services;

    LSPHandle _lspHandle;

    VirtualFileSystem _vfs;
};

template<ServiceClass Service>
Service *LanguageServer::GetService() {
    return dynamic_cast<Service *>(_services[static_cast<std::size_t>(Service::ServiceIndex)].get());
}

template<ServiceClass Service, typename ... ARGS>
void LanguageServer::AddService(ARGS &&... args) {
    auto index = Service::ServiceIndex;
    auto service = std::make_unique<Service>(this, std::forward<ARGS &&>(args)...);
    _services[static_cast<std::size_t>(index)] = std::move(service);
}
