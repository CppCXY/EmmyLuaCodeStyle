#include "CodeFormatServer/Service/CommandService.h"

#include "CodeFormatServer/Service/CodeFormatService.h"
#include "CodeFormatServer/Service/ModuleService.h"
#include "CodeService/LuaFormatRange.h"
#include "Util/format.h"
#include "Util/Url.h"

CommandService::CommandService(std::shared_ptr<LanguageClient> owner)
	: Service(owner)
{
}

void CommandService::Start()
{
}

bool CommandService::Initialize()
{
	CommandProtocol("emmylua.reformat.me", &CommandService::Reformat);
	CommandProtocol("emmylua.import.me", &CommandService::Import);
	CommandProtocol("emmylua.spell.correct", &CommandService::SpellCorrect);
	// CommandProtocol("emmylua.spell.addDict", &CommandService::SpellAddDict);

	return true;
}

std::vector<std::string> CommandService::GetCommands()
{
	std::vector<std::string> results;
	for (auto it : _handles)
	{
		results.push_back(it.first);
	}

	return results;
}

std::string CommandService::GetCommand(Command command)
{
	switch (command)
	{
	case Command::Import: return "emmylua.import.me";
	case Command::Reformat: return "emmylua.reformat.me";
	case Command::SpellCorrect: return "emmylua.spell.correct";
	case Command::SpellAddDict: return "emmylua.spell.addDict";
	}

	return "";
}

void CommandService::Dispatch(std::string_view command, std::shared_ptr<vscode::ExecuteCommandParams> param)
{
	std::string cmd(command);
	auto it = _handles.find(cmd);
	if (it != _handles.end())
	{
		it->second(param);
	}
}

void CommandService::Reformat(std::shared_ptr<vscode::ExecuteCommandParams> param)
{
	if (param->arguments.size() < 2)
	{
		return;
	}

	std::string uri = param->arguments[0];
	vscode::Range range;

	range.Deserialize(param->arguments[1]);

	auto parser = LanguageClient::GetInstance().GetFileParser(uri);

	auto applyParams = std::make_shared<vscode::ApplyWorkspaceEditParams>();

	auto options = LanguageClient::GetInstance().GetOptions(uri);

	if (parser->HasError())
	{
		return;
	}

	auto it = applyParams->edit.changes.emplace(uri, std::vector<vscode::TextEdit>());
	auto& change = it.first->second;

	auto& edit = change.emplace_back();
	LuaFormatRange formattedRange(static_cast<int>(range.start.line), static_cast<int>(range.end.line));

	auto formatResult = GetService<CodeFormatService>()->RangeFormat(formattedRange, parser, options);

	edit.newText = std::move(formatResult);

	edit.range = vscode::Range(
		vscode::Position(formattedRange.StartLine, formattedRange.StartCharacter),
		vscode::Position(formattedRange.EndLine + 1, formattedRange.EndCharacter)
	);

	_owner->SendRequest("workspace/applyEdit", applyParams);
}

void CommandService::Import(std::shared_ptr<vscode::ExecuteCommandParams> param)
{
	if (param->arguments.size() < 4)
	{
		return;
	}

	std::string uri = param->arguments[0];
	std::string filePath = url::UrlToFilePath(uri);
	auto config = GetService<ModuleService>()->GetIndex().GetConfig(filePath);
	if (!config)
	{
		return;
	}
	vscode::Range range;

	range.Deserialize(param->arguments[1]);

	std::string moduleName = param->arguments[2];

	std::string moduleDefineName = param->arguments[3];

	std::string requireString = Util::format("local {} = {}(\"{}\")\n", moduleDefineName, config->import_function,
	                                         moduleName);
	auto parser = _owner->GetFileParser(uri);
	auto applyParams = std::make_shared<vscode::ApplyWorkspaceEditParams>();
	auto it = applyParams->edit.changes.emplace(uri, std::vector<vscode::TextEdit>());
	auto& change = it.first->second;

	auto& edit = change.emplace_back();

	edit.newText = requireString;

	edit.range = GetService<ModuleService>()->FindRequireRange(parser, config);

	_owner->SendRequest("workspace/applyEdit", applyParams);
}

void CommandService::SpellCorrect(std::shared_ptr<vscode::ExecuteCommandParams> param)
{
	std::string uri = param->arguments[0];
	vscode::Range range;

	range.Deserialize(param->arguments[1]);

	std::string newText = param->arguments[2];

	auto applyParams = std::make_shared<vscode::ApplyWorkspaceEditParams>();
	auto it = applyParams->edit.changes.emplace(uri, std::vector<vscode::TextEdit>());
	auto& change = it.first->second;

	auto& edit = change.emplace_back();

	edit.newText = newText;

	edit.range = range;

	_owner->SendRequest("workspace/applyEdit", applyParams);
}

// void CommandService::SpellAddDict(std::shared_ptr<vscode::ExecuteCommandParams> param)
// {
// }
