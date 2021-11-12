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
	else if(method == "textDocument/didChange")
	{
		return MakeRequestObject<DidChangeTextDocumentParams>(json);
	}

	return nullptr;
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

	return object;
}

void vscode::TextDocumentSyncOptions::Deserialize(nlohmann::json json)
{
	openClose = json["openClose"];
	change = static_cast<TextDocumentSyncKind>(json["change"].get<int>());
}

nlohmann::json vscode::ServerCapabilities::Serialize()
{
	auto object = nlohmann::json::object();
	object["textDocumentSync"] = textDocumentSync.Serialize();
	object["documentFormattingProvider"] = documentFormattingProvider;

	return object;
}

void vscode::ServerCapabilities::Deserialize(nlohmann::json json)
{
	documentFormattingProvider = json["documentFormattingProvider"];
	textDocumentSync.Deserialize(json["textDocumentSync"]);
}

nlohmann::json vscode::InitializeParams::Serialize()
{
	auto object = nlohmann::json::object();
	object["rootPath"] = rootPath;
	object["rootUri"] = rootUri;
	object["locale"] = locale;
	object["initializationOptions"] = initializationOptions;

	return object;
}

void vscode::InitializeParams::Deserialize(nlohmann::json json)
{
	rootPath = json["rootPath"];
	rootUri = json["rootUri"];
	locale = json["locale"];

	initializationOptions = json["initializationOptions"];
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
