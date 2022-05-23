#include "CodeFormatServer/VSCode.h"


vscode::Serializable::~Serializable()
{
}

vscode::Position::Position(uint64_t _line, uint64_t _character)
	: line(_line),
	  character(_character)
{
}

nlohmann::json vscode::Position::Serialize()
{
	auto object = nlohmann::json::object();
	object["line"] = line;
	object["character"] = character;

	return object;
}

void vscode::Position::Deserialize(nlohmann::json json)
{
	line = json["line"];
	character = json["character"];
}

vscode::Range::Range(Position _start, Position _end)
	: start(_start),
	  end(_end)
{
}

nlohmann::json vscode::Range::Serialize()
{
	auto object = nlohmann::json::object();
	object["start"] = start.Serialize();
	object["end"] = end.Serialize();

	return object;
}

void vscode::Range::Deserialize(nlohmann::json json)
{
	start.Deserialize(json["start"]);
	end.Deserialize(json["end"]);
}

nlohmann::json vscode::Location::Serialize()
{
	auto object = nlohmann::json::object();
	object["range"] = range.Serialize();
	object["uri"] = uri;

	return object;
}

void vscode::Location::Deserialize(nlohmann::json json)
{
	range.Deserialize(json["range"]);
	uri = json["uri"];
}

vscode::Diagnostic::Diagnostic()
{
}

nlohmann::json vscode::Diagnostic::Serialize()
{
	auto object = nlohmann::json::object();
	object["message"] = message;
	object["range"] = range.Serialize();
	object["severity"] = static_cast<int>(severity);
	object["data"] = data;
	object["code"] = code;
	return object;
}

void vscode::Diagnostic::Deserialize(nlohmann::json json)
{
	message = json["message"];
	range.Deserialize(json["range"]);
	severity = static_cast<DiagnosticSeverity>(json["severity"].get<int>());
	if (json["data"].is_string())
	{
		data = json["data"];
	}
	if (json["code"].is_string())
	{
		code = json["code"];
	}
}

nlohmann::json vscode::TextDocument::Serialize()
{
	auto object = nlohmann::json::object();
	object["uri"] = uri;

	return object;
}

void vscode::TextDocument::Deserialize(nlohmann::json json)
{
	uri = json["uri"];
}

nlohmann::json vscode::TextEdit::Serialize()
{
	auto object = nlohmann::json::object();
	object["range"] = range.Serialize();
	object["newText"] = newText;

	return object;
}

nlohmann::json vscode::PublishDiagnosticsParams::Serialize()
{
	auto object = nlohmann::json::object();
	object["uri"] = uri;

	auto array = nlohmann::json::array();

	for (auto& diagnositc : diagnostics)
	{
		array.push_back(diagnositc.Serialize());
	}

	object["diagnostics"] = array;

	return object;
}

void vscode::PublishDiagnosticsParams::Deserialize(nlohmann::json json)
{
	uri = json["uri"];
	auto dianosticsArray = json["diagnostics"];

	for (auto diagnostic : dianosticsArray)
	{
		diagnostics.emplace_back();
		diagnostics.back().Deserialize(diagnostic);
	}
}

nlohmann::json vscode::TextDocumentSyncOptions::Serialize()
{
	auto object = nlohmann::json::object();

	object["openClose"] = openClose;
	object["change"] = change;
	object["willSave"] = willSave;
	object["willSaveWaitUntil"] = willSaveWaitUntil;

	return object;
}

nlohmann::json vscode::ServerCapabilities::Serialize()
{
	auto object = nlohmann::json::object();
	object["textDocumentSync"] = textDocumentSync.Serialize();
	object["documentFormattingProvider"] = documentFormattingProvider;
	object["documentRangeFormattingProvider"] = documentRangeFormattingProvider;
	object["documentOnTypeFormattingProvider"] = documentOnTypeFormattingProvider.Serialize();
	object["codeActionProvider"] = codeActionProvider;
	object["executeCommandProvider"] = executeCommandProvider.Serialize();
	object["diagnosticProvider"] = diagnosticProvider.Serialize();
	if (completionProvider.supportCompletion)
	{
		object["completionProvider"] = completionProvider.Serialize();
	}


	return object;
}

void vscode::InitializationOptions::Deserialize(nlohmann::json json)
{
	if (json["editorConfigFiles"].is_array())
	{
		auto configFileArray = json["editorConfigFiles"];

		for (auto& configSource : configFileArray)
		{
			auto& source = editorConfigFiles.emplace_back();
			source.Deserialize(configSource);
		}
	}
	if (json["moduleConfigFiles"].is_array())
	{
		auto configFileArray = json["moduleConfigFiles"];

		for (auto& configSource : configFileArray)
		{
			auto& source = moduleConfigFiles.emplace_back();
			source.Deserialize(configSource);
		}
	}

	if (json["workspaceFolders"].is_array())
	{
		auto workspaceFolderArray = json["workspaceFolders"];

		for (auto& workspaceUri : workspaceFolderArray)
		{
			workspaceFolders.emplace_back(workspaceUri);
		}
	}
	if (json["localeRoot"].is_string())
	{
		localeRoot = json["localeRoot"];
	}

	if (json["extensionChars"].is_string())
	{
		extensionChars = json["extensionChars"];
	}

	if (json["vscodeConfig"].is_object())
	{
		vscodeConfig.Deserialize(json["vscodeConfig"]);
	}

	if (json["dictionaryPath"].is_array())
	{
		for (auto& j : json["dictionaryPath"])
		{
			dictionaryPath.push_back(j);
		}
	}
}

void vscode::InitializeParams::Deserialize(nlohmann::json json)
{
	if (json["rootPath"].is_string())
	{
		rootPath = json["rootPath"];
	}
	if (json["rootUri"].is_string())
	{
		rootUri = json["rootUri"];
	}
	if (json["locale"].is_string())
	{
		locale = json["locale"];
	}

	initializationOptions.Deserialize(json["initializationOptions"]);
}

nlohmann::json vscode::InitializeResult::Serialize()
{
	auto object = nlohmann::json::object();

	object["capabilities"] = capabilities.Serialize();
	return object;
}

void vscode::InitializeResult::Deserialize(nlohmann::json json)
{
	capabilities.Deserialize(json["capabilities"]);
}

void vscode::TextDocumentContentChangeEvent::Deserialize(nlohmann::json json)
{
	text = json["text"];
	if (!json["range"].is_null())
	{
		auto textRange = Range();
		textRange.Deserialize(json["range"]);
		range = textRange;
	}
}

void vscode::DidChangeTextDocumentParams::Deserialize(nlohmann::json json)
{
	auto contentChangeArray = json["contentChanges"];

	for (auto contentChange : contentChangeArray)
	{
		contentChanges.emplace_back();
		contentChanges.back().Deserialize(contentChange);
	}

	textDocument.Deserialize(json["textDocument"]);
}

void vscode::TextDocumentItem::Deserialize(nlohmann::json json)
{
	uri = json["uri"];

	languageId = json["languageId"];

	text = json["text"];
}

void vscode::DidOpenTextDocumentParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
}

void vscode::DocumentFormattingParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
}

nlohmann::json vscode::DocumentFormattingResult::Serialize()
{
	if (hasError)
	{
		return nullptr;
	}
	else
	{
		auto array = nlohmann::json::array();
		for (auto& edit : edits)
		{
			array.push_back(edit.Serialize());
		}

		return array;
	}
}

void vscode::DidCloseTextDocumentParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
}

void vscode::ConfigSource::Deserialize(nlohmann::json json)
{
	uri = json["uri"];
	path = json["path"];
	workspace = json["workspace"];
}

void vscode::VscodeSettings::Deserialize(nlohmann::json json)
{
	if (json["emmylua.lint.moduleCheck"].is_boolean())
	{
		lintModule = json["emmylua.lint.moduleCheck"];
	}

	if (json["emmylua.lint.codeStyle"].is_boolean())
	{
		lintCodeStyle = json["emmylua.lint.codeStyle"];
	}

	if (json["emmylua.spell.enable"].is_boolean())
	{
		spellEnable = json["emmylua.spell.enable"];
	}

	if (json["emmylua.spell.dict"].is_array())
	{
		spellDict.clear();
		for (auto j : json["emmylua.spell.dict"])
		{
			spellDict.push_back(j);
		}
	}
}

void vscode::ConfigUpdateParams::Deserialize(nlohmann::json json)
{
	type = json["type"];
	source.Deserialize(json["source"]);
}

void vscode::DocumentRangeFormattingParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
	range.Deserialize(json["range"]);
}

nlohmann::json vscode::DocumentOnTypeFormattingOptions::Serialize()
{
	auto object = nlohmann::json::object();

	object["firstTriggerCharacter"] = firstTriggerCharacter;
	auto array = nlohmann::json::array();

	for (auto& c : moreTriggerCharacter)
	{
		array.push_back(c);
	}

	object["moreTriggerCharacter"] = array;
	return object;
}

nlohmann::json vscode::ExecuteCommandOptions::Serialize()
{
	auto object = nlohmann::json::object();
	auto array = nlohmann::json::array();
	for (auto& command : commands)
	{
		array.push_back(command);
	}
	object["commands"] = array;

	return object;
}

nlohmann::json vscode::CompletionItemOptions::Serialize()
{
	auto object = nlohmann::json::object();

	object["labelDetailsSupport"] = labelDetailsSupport;
	return object;
}

nlohmann::json vscode::CompletionOptions::Serialize()
{
	auto object = nlohmann::json::object();
	object["triggerCharacters"] = SerializeArray(triggerCharacters);
	object["resolveProvider"] = resolveProvider;
	object["completionItem"] = completionItem.Serialize();
	return object;
}

nlohmann::json vscode::DiagnosticOptions::Serialize()
{
	auto object = nlohmann::json::object();
	object["identifier"] = identifier;
	object["workspaceDiagnostics"] = workspaceDiagnostics;
	object["interFileDependencies"] = interFileDependencies;
	return object;
}

void vscode::TextDocumentPositionParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
	position.Deserialize(json["position"]);
}

void vscode::CodeActionContext::Deserialize(nlohmann::json json)
{
	auto jsonDiagnostics = json["diagnostics"];
	if (jsonDiagnostics.is_array())
	{
		for (auto& diagnostic : jsonDiagnostics)
		{
			diagnostics.emplace_back().Deserialize(diagnostic);
		}
	}
}

void vscode::CodeActionParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
	range.Deserialize(json["range"]);
	context.Deserialize(json["context"]);
}

nlohmann::json vscode::Command::Serialize()
{
	auto object = nlohmann::json::object();

	object["title"] = title;
	object["command"] = command;

	auto array = nlohmann::json::array();

	for (auto& arg : arguments)
	{
		array.push_back(arg);
	}

	object["arguments"] = array;

	return object;
}

nlohmann::json vscode::CodeAction::Serialize()
{
	auto object = nlohmann::json::object();

	object["title"] = title;
	object["command"] = command.Serialize();
	object["kind"] = kind;
	return object;
}

nlohmann::json vscode::CodeActionResult::Serialize()
{
	auto array = nlohmann::json::array();

	for (auto& action : actions)
	{
		array.push_back(action.Serialize());
	}

	return array;
}

void vscode::ExecuteCommandParams::Deserialize(nlohmann::json json)
{
	command = json["command"];
	auto args = json["arguments"];
	if (args.is_array())
	{
		for (auto arg : args)
		{
			if (!arg.is_null())
			{
				arguments.push_back(arg);
			}
		}
	}
}

nlohmann::json vscode::OptionalVersionedTextDocumentIdentifier::Serialize()
{
	auto object = TextDocument::Serialize();
	if (version.has_value())
	{
		object["version"] = version.value();
	}
	else
	{
		object["version"] = nullptr;
	}

	return object;
}

nlohmann::json vscode::TextDocumentEdit::Serialize()
{
	auto object = nlohmann::json::object();

	object["textDocument"] = textDocument.Serialize();

	object["edits"] = SerializeArray(edits);

	return object;
}

nlohmann::json vscode::WorkspaceEdit::Serialize()
{
	auto object = nlohmann::json::object();

	auto changesObject = nlohmann::json::object();

	for (auto it : changes)
	{
		changesObject[it.first] = SerializeArray(it.second);
	}
	object["changes"] = changesObject;

	return object;
}

nlohmann::json vscode::ApplyWorkspaceEditParams::Serialize()
{
	auto object = nlohmann::json::object();

	if (label.has_value())
	{
		object["label"] = label.value();
	}
	else
	{
		object["label"] = nullptr;
	}

	object["edit"] = edit.Serialize();

	return object;
}

void vscode::FileEvent::Deserialize(nlohmann::json json)
{
	uri = json["uri"];
	type = static_cast<FileChangeType>(static_cast<uint32_t>(json["type"]));
}

void vscode::DidChangeWatchedFilesParams::Deserialize(nlohmann::json json)
{
	if (json["changes"].is_array())
	{
		for (auto& v : json["changes"])
		{
			changes.emplace_back().Deserialize(v);
		}
	}
}

void vscode::CompletionParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
	position.Deserialize(json["position"]);
}


nlohmann::json vscode::CompletionItemLabelDetails::Serialize()
{
	auto object = nlohmann::json::object();

	object["detail"] = detail;

	if (description.has_value())
	{
		object["description"] = description.value();
	}

	return object;
}

nlohmann::json vscode::CompletionItem::Serialize()
{
	auto object = nlohmann::json::object();

	object["label"] = label;
	object["kind"] = static_cast<int>(kind);
	object["detail"] = detail;
	object["documentation"] = documentation;
	object["insertText"] = insertText;

	if (sortText.has_value())
	{
		object["sortText"] = sortText.value();
	}
	if (filterText.has_value())
	{
		object["filterText"] = filterText.value();
	}

	if (command.has_value())
	{
		object["command"] = command->Serialize();
	}

	if (labelDetails.has_value())
	{
		object["labelDetails"] = labelDetails->Serialize();
	}

	return object;
}

nlohmann::json vscode::CompletionList::Serialize()
{
	auto object = nlohmann::json::object();
	object["isIncomplete"] = isIncomplete;
	object["items"] = SerializeArray(items);

	return object;
}

void vscode::DidChangeConfigurationParams::Deserialize(nlohmann::json json)
{
	if (json["settings"].is_object())
	{
		settings = json["settings"];
	}
}

void vscode::DocumentDiagnosticParams::Deserialize(nlohmann::json json)
{
	if (json["textDocument"].is_object())
	{
		textDocument.Deserialize(json["textDocument"]);
	}

	if (json["identifier"].is_string())
	{
		identifier = json["identifier"];
	}

	if (json["previousResultId"].is_string())
	{
		previousResultId = json["previousResultId"];
	}
}

nlohmann::json vscode::DocumentDiagnosticReport::Serialize()
{
	auto object = nlohmann::json::object();
	object["kind"] = kind;
	if(kind == DocumentDiagnosticReportKind::Full)
	{
		object["items"] = SerializeArray(items);
	}

	if(!resultId.empty())
	{
		object["resultId"] = resultId;
	}

	return object;
}

void vscode::PreviousResultId::Deserialize(nlohmann::json json)
{
	if (json["uri"].is_string())
	{
		uri = json["uri"];
	}

	if (json["value"].is_string())
	{
		value = json["value"];
	}
}

void vscode::WorkspaceDiagnosticParams::Deserialize(nlohmann::json json)
{
	if (json["identifier"].is_string())
	{
		identifier = json["identifier"];
	}

	if (json["previousResultIds"].is_array())
	{
		for (auto j : json["previousResultIds"])
		{
			if (j.is_object())
			{
				previousResultIds.emplace_back().Deserialize(j);
			}
		}
	}
}

nlohmann::json vscode::WorkspaceDocumentDiagnosticReport::Serialize()
{
	auto object = DocumentDiagnosticReport::Serialize();
	object["uri"] = uri;
	if(version.has_value())
	{
		object["version"] = version.value();
	}
	else
	{
		object["version"] = nullptr;
	}

	return object;
}

nlohmann::json vscode::WorkspaceDiagnosticReport::Serialize()
{
	auto object = nlohmann::json::object();
	object["items"] = SerializeArray(items);
	return object;
}
