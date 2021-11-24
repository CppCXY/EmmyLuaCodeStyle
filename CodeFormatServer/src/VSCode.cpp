#include "CodeFormatServer/VSCode.h"

template <class T>
std::shared_ptr<vscode::Serializable> MakeRequestObject(nlohmann::json json)
{
	auto object = std::make_shared<T>();
	object->Deserialize(json);

	return object;
}

vscode::Serializable::~Serializable()
{
}

std::shared_ptr<vscode::Serializable> vscode::MakeFromRequest(std::string_view method, nlohmann::json json)
{
	if (method == "initialize")
	{
		return MakeRequestObject<InitializeParams>(json);
	}
	else if (method == "textDocument/didChange")
	{
		return MakeRequestObject<DidChangeTextDocumentParams>(json);
	}
	else if (method == "textDocument/didOpen")
	{
		return MakeRequestObject<DidOpenTextDocumentParams>(json);
	}
	else if (method == "textDocument/formatting")
	{
		return MakeRequestObject<DocumentFormattingParams>(json);
	}
	else if (method == "textDocument/didClose")
	{
		return MakeRequestObject<DidCloseTextDocumentParams>(json);
	}
	else if (method == "updateEditorConfig")
	{
		return MakeRequestObject<EditorConfigUpdateParams>(json);
	}
	else if (method == "textDocument/rangeFormatting")
	{
		return MakeRequestObject<DocumentRangeFormattingParams>(json);
	}
	else if (method == "textDocument/onTypeFormatting")
	{
		return MakeRequestObject<TextDocumentPositionParams>(json);
	}

	return nullptr;
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

	return object;
}

void vscode::Diagnostic::Deserialize(nlohmann::json json)
{
	message = json["message"];
	range.Deserialize(json["range"]);
	severity = static_cast<DiagnosticSeverity>(json["severity"].get<int>());
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

	return object;
}

void vscode::InitializationOptions::Deserialize(nlohmann::json json)
{
	if (json["configFiles"].is_array())
	{
		auto configFileArray = json["configFiles"];

		for (auto& configSource : configFileArray)
		{
			auto& source = configFiles.emplace_back();
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

nlohmann::json vscode::TextDocumentContentChangeEvent::Serialize()
{
	auto object = nlohmann::json::object();

	object["text"] = text;
	return object;
}

void vscode::TextDocumentContentChangeEvent::Deserialize(nlohmann::json json)
{
	text = json["text"];
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

void vscode::EditorConfigSource::Deserialize(nlohmann::json json)
{
	uri = json["uri"];
	path = json["path"];
	workspace = json["workspace"];
}

void vscode::EditorConfigUpdateParams::Deserialize(nlohmann::json json)
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

void vscode::TextDocumentPositionParams::Deserialize(nlohmann::json json)
{
	textDocument.Deserialize(json["textDocument"]);
	position.Deserialize(json["position"]);
}
