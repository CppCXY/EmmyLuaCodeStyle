#include "CommandService.h"

#include "FormatService.h"
#include "Util/format.h"
#include "Util/Url.h"

CommandService::CommandService(LanguageServer* owner)
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

void CommandService::Dispatch(std::string_view command, std::shared_ptr<lsp::ExecuteCommandParams> param)
{
	std::string cmd(command);
	auto it = _handles.find(cmd);
	if (it != _handles.end())
	{
		it->second(param);
	}
}

void CommandService::Reformat(std::shared_ptr<lsp::ExecuteCommandParams> param)
{
	if (param->arguments.size() < 2)
	{
		return;
	}

	std::string uri = param->arguments[0];
	lsp::Range range;

	range.Deserialize(param->arguments[1]);

	auto parser = LanguageServer::GetInstance().GetFileParser(uri);

	auto applyParams = std::make_shared<lsp::ApplyWorkspaceEditParams>();

	auto options = LanguageServer::GetInstance().GetOptions(uri);

	if (parser->HasError())
	{
		return;
	}

	auto it = applyParams->edit.changes.emplace(uri, std::vector<lsp::TextEdit>());
	auto& change = it.first->second;

	auto& edit = change.emplace_back();
	LuaFormatRange formattedRange(static_cast<int>(range.start.line), static_cast<int>(range.end.line));

	auto formatResult = GetService<FormatService>()->RangeFormat(formattedRange, parser, *options);

	edit.newText = std::move(formatResult);

	edit.range = lsp::Range(
            lsp::Position(formattedRange.StartLine, formattedRange.StartCharacter),
            lsp::Position(formattedRange.EndLine + 1, formattedRange.EndCharacter)
	);

	_owner->SendRequest("workspace/applyEdit", applyParams);
}

void CommandService::Import(std::shared_ptr<lsp::ExecuteCommandParams> param)
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
	lsp::Range range;

	range.Deserialize(param->arguments[1]);

	std::string moduleName = param->arguments[2];

	std::string moduleDefineName = param->arguments[3];

	std::string requireString = Util::format("local {} = {}(\"{}\")\n", moduleDefineName, config->import_function,
	                                         moduleName);
	auto parser = _owner->GetFileParser(uri);
	auto applyParams = std::make_shared<lsp::ApplyWorkspaceEditParams>();
	auto it = applyParams->edit.changes.emplace(uri, std::vector<lsp::TextEdit>());
	auto& change = it.first->second;

	auto& edit = change.emplace_back();

	edit.newText = requireString;

	edit.range = GetService<ModuleService>()->FindRequireRange(parser, config);

	_owner->SendRequest("workspace/applyEdit", applyParams);
}

void CommandService::SpellCorrect(std::shared_ptr<lsp::ExecuteCommandParams> param)
{
	std::string uri = param->arguments[0];
	lsp::Range range;

	range.Deserialize(param->arguments[1]);

	std::string newText = param->arguments[2];

	auto applyParams = std::make_shared<lsp::ApplyWorkspaceEditParams>();
	auto it = applyParams->edit.changes.emplace(uri, std::vector<lsp::TextEdit>());
	auto& change = it.first->second;

	auto& edit = change.emplace_back();

	edit.newText = newText;

	edit.range = range;

	_owner->SendRequest("workspace/applyEdit", applyParams);
}

// void CommandService::SpellAddDict(std::shared_ptr<lsp::ExecuteCommandParams> param)
// {
// }
