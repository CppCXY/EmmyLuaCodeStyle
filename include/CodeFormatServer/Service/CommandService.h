#pragma once

#include "Service.h"

class CommandService : public Service
{
public:
	LANGUAGE_SERVICE(CommandService);

	enum class Command
	{
		Reformat = 1,
		Import = 2,
		SpellCorrect = 3,
		SpellAddDict = 4
	};

	using CommandHandle = std::function<void(std::shared_ptr<vscode::ExecuteCommandParams> param)>;

	CommandService(std::shared_ptr<LanguageClient> owner);

	void Start() override;

	bool Initialize() override;

	std::vector<std::string> GetCommands();

	std::string GetCommand(Command command);

	void Dispatch(std::string_view command, std::shared_ptr<vscode::ExecuteCommandParams> param);
private:

	void CommandProtocol(std::string_view command, void(CommandService::* handle)(std::shared_ptr<vscode::ExecuteCommandParams>))
	{
		_handles[std::string(command)] = [this, handle](auto params) {
			return (this->*handle)(params);
		};
	}

	void Reformat(std::shared_ptr<vscode::ExecuteCommandParams> param);

	void Import(std::shared_ptr<vscode::ExecuteCommandParams> param);

	void SpellCorrect(std::shared_ptr<vscode::ExecuteCommandParams> param);

	void SpellAddDict(std::shared_ptr<vscode::ExecuteCommandParams> param);

	std::map<std::string, CommandHandle> _handles;
};
