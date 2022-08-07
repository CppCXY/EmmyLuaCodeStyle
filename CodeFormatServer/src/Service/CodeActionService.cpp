#include "CodeFormatServer/Service/CodeActionService.h"

#include "CodeFormatServer/Service/CodeFormatService.h"
#include "CodeFormatServer/Service/CommandService.h"
#include "CodeFormatServer/Service/ModuleService.h"
#include "Util/Url.h"
#include "Util/format.h"

CodeActionService::CodeActionService(std::shared_ptr<LanguageClient> owner)
	: Service(owner)
{
}

bool CodeActionService::Initialize()
{
	CodeProtocol(DiagnosticCode::Spell, &CodeActionService::Spell);
	CodeProtocol(DiagnosticCode::Reformat, &CodeActionService::Reformat);
	CodeProtocol(DiagnosticCode::Module, &CodeActionService::Module);

	return true;
}

void CodeActionService::Dispatch(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
                                 std::shared_ptr<vscode::CodeActionResult> results)
{
	if (diagnostic.code.empty())
	{
		return;
	}

	auto it = _handles.find(diagnostic.code);
	if (it != _handles.end())
	{
		it->second(diagnostic, param, results);
	}
}


std::string CodeActionService::GetCode(DiagnosticCode code)
{
	switch (code)
	{
	case DiagnosticCode::Spell: return "spell-check";
	case DiagnosticCode::Module: return "module-check";
	case DiagnosticCode::Reformat: return "style-check";
	}

	return "";
}

void CodeActionService::CodeProtocol(DiagnosticCode code, CodeActionHandleMember handle)
{
	auto command = GetCode(code);
	_handles[command] = [this, handle](auto diagnostic, auto params, auto result)
	{
		return (this->*handle)(diagnostic, params, result);
	};
}

void CodeActionService::Reformat(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
                                 std::shared_ptr<vscode::CodeActionResult> result)
{
	auto& action = result->actions.emplace_back();
	std::string title = "reformat current line";
	action.title = title;
	action.command.title = title;
	action.command.command = GetService<CommandService>()->GetCommand(CommandService::Command::Reformat);
	action.command.arguments.push_back(param->textDocument.uri);
	action.command.arguments.push_back(param->range.Serialize());

	action.kind = vscode::CodeActionKind::QuickFix;
}

void CodeActionService::Spell(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
                              std::shared_ptr<vscode::CodeActionResult> result)
{
	auto& originText = diagnostic.data;
	if (originText.empty())
	{
		return;
	}

	// add to custom dictionary
	{
		auto& action = result->actions.emplace_back();

		action.title = Util::format("Add '{}' to workspace dictionary", originText);
		action.command.title = action.title;
		action.command.command = GetService<CommandService>()->GetCommand(CommandService::Command::SpellAddDict);
		action.command.arguments.push_back(originText);

		action.kind = vscode::CodeActionKind::QuickFix;
	}

	auto spellChecker = GetService<CodeFormatService>()->GetSpellChecker();
	auto suggests = spellChecker->GetSuggests(originText);
	for (auto& suggest : suggests)
	{
		if (!suggest.Term.empty())
		{
			auto& action = result->actions.emplace_back();
			auto& term = suggest.Term;

			action.title = term;
			action.command.title = term;
			action.command.command = GetService<CommandService>()->GetCommand(CommandService::Command::SpellCorrect);
			action.command.arguments.push_back(param->textDocument.uri);
			action.command.arguments.push_back(diagnostic.range.Serialize());
			action.command.arguments.push_back(term);

			action.kind = vscode::CodeActionKind::QuickFix;
		}
	}
}

void CodeActionService::Module(vscode::Diagnostic& diagnostic, std::shared_ptr<vscode::CodeActionParams> param,
                               std::shared_ptr<vscode::CodeActionResult> result)
{
	auto range = param->range;
	auto uri = param->textDocument.uri;
	auto filePath = url::UrlToFilePath(uri);
	auto modules = GetService<ModuleService>()->GetImportModules(filePath, range);
	auto& action = result->actions.emplace_back();
	std::string title = "import multi choice";
	if (modules.size() == 1)
	{
		title = Util::format("import '{}'", modules.front().ModuleName);
	}
	action.title = title;
	action.command.title = title;
	action.command.command = GetService<CommandService>()->GetCommand(CommandService::Command::Import);
	action.command.arguments.push_back(uri);
	action.command.arguments.push_back(range.Serialize());
	for (auto& module : modules)
	{
		auto object = nlohmann::json::object();
		object["moduleName"] = module.ModuleName;
		object["path"] = module.FilePath;
		object["name"] = module.Name;
		action.command.arguments.push_back(object);
	}

	action.kind = vscode::CodeActionKind::QuickFix;
}
