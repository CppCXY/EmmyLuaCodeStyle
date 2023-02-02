#pragma once

#include "Service.h"
#include "LSP/LSP.h"
#include <map>
#include <functional>
#include <string_view>

class CommandService : public Service {
public:
    LANGUAGE_SERVICE(CommandService);

    enum class Command {
        Reformat = 1,
        SpellCorrect,
        SpellAddDict
    };

    using CommandHandle = std::function<void(std::shared_ptr<lsp::ExecuteCommandParams> params)>;

    explicit CommandService(LanguageServer *owner);

    void Start() override;

    bool Initialize() override;

    std::vector<std::string> GetCommands();

    std::string GetCommand(Command command);

    void Dispatch(std::string_view command, std::shared_ptr<lsp::ExecuteCommandParams> params);

private:

    void CommandProtocol(std::string_view command,
                         void(CommandService::* handle)(std::shared_ptr<lsp::ExecuteCommandParams>)) {
        _handles[std::string(command)] = [this, handle](auto params) {
            return (this->*handle)(params);
        };
    }

    void Reformat(std::shared_ptr<lsp::ExecuteCommandParams> params);

    void SpellCorrect(std::shared_ptr<lsp::ExecuteCommandParams> params);

    std::map<std::string, CommandHandle, std::less<>> _handles;
};
