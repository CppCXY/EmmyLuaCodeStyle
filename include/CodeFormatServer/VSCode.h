#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
// 命名风格和vscode一样

namespace vscode
{

class Serializable
{
public:
	virtual ~Serializable();

	virtual nlohmann::json Serialize()
	{
		auto object = nlohmann::json::object();
		return object;
	}

	virtual void Deserialize(nlohmann::json json)
	{
	};
};

std::shared_ptr<Serializable> MakeFromRequest(std::string_view method, nlohmann::json json);

class Position : public Serializable
{
public:
	Position(uint64_t _line = 0, uint64_t _character = 0);

	uint64_t line;
	uint64_t character;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class Range : public Serializable
{
public:
	Range(Position _start = Position(0, 0), Position _end = Position(0, 0));
	Position start;
	Position end;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class Location : public Serializable
{
public:
	std::string uri;
	Range range;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

enum class DiagnosticSeverity
{
	Error = 1,
	Warning = 2,
	Information = 3,
	Hint = 4
};

class Diagnostic : public Serializable
{
public:
	Diagnostic();
	Range range;
	DiagnosticSeverity severity;
	std::string message;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class TextDocument : public Serializable
{
public:
	std::string uri;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class TextEdit : public Serializable
{
public:
	Range range;
	std::string newText;

	nlohmann::json Serialize() override;
};


class PublishDiagnosticsParams : public Serializable
{
public:
	std::string uri;

	std::vector<Diagnostic> diagnostics;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

enum class TextDocumentSyncKind
{
	None = 0,
	Full = 1,
	Incremental = 2
};

class TextDocumentSyncOptions : public Serializable
{
public:
	bool openClose = false;
	TextDocumentSyncKind change = TextDocumentSyncKind::None;
	bool willSave = false;
	bool willSaveWaitUntil = false;

	nlohmann::json Serialize() override;
};

class DocumentOnTypeFormattingOptions : public Serializable
{
public:
	std::string firstTriggerCharacter;
	std::vector<std::string> moreTriggerCharacter;

	nlohmann::json Serialize() override;
};

class ServerCapabilities : public Serializable
{
public:
	TextDocumentSyncOptions textDocumentSync;
	bool documentFormattingProvider = false;
	bool documentRangeFormattingProvider = false;
	DocumentOnTypeFormattingOptions documentOnTypeFormattingProvider;

	nlohmann::json Serialize() override;
};

class EditorConfigSource;

class InitializationOptions : public Serializable
{
public:
	std::vector<std::string> workspaceFolders;
	std::vector<EditorConfigSource> configFiles;

	void Deserialize(nlohmann::json json) override;
};

class InitializeParams : public Serializable
{
public:
	std::string rootUri;
	std::string rootPath;
	std::string locale;

	InitializationOptions initializationOptions;
	// 其他参数忽略
	void Deserialize(nlohmann::json json) override;
};

class InitializeResult : public Serializable
{
public:
	ServerCapabilities capabilities;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class TextDocumentContentChangeEvent : public Serializable
{
public:
	std::string text;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class DidChangeTextDocumentParams : public Serializable
{
public:
	TextDocument textDocument;
	std::vector<TextDocumentContentChangeEvent> contentChanges;

	void Deserialize(nlohmann::json json) override;
};

class TextDocumentItem : public Serializable
{
public:
	std::string uri;
	std::string languageId;
	// int version;
	std::string text;

	void Deserialize(nlohmann::json json) override;
};

class DidOpenTextDocumentParams : public Serializable
{
public:
	TextDocumentItem textDocument;

	void Deserialize(nlohmann::json json) override;
};

class DocumentFormattingParams : public Serializable
{
public:
	TextDocument textDocument;

	void Deserialize(nlohmann::json json) override;
};

// 该类并非language server protocol 接口
class DocumentFormattingResult : public Serializable
{
public:
	std::vector<TextEdit> edits;

	bool hasError = false;

	nlohmann::json Serialize() override;
};

class DidCloseTextDocumentParams : public Serializable
{
public:
	TextDocument textDocument;

	void Deserialize(nlohmann::json json) override;
};

enum class EditorConfigUpdateType
{
	Created,
	Changed,
	Delete
};

class EditorConfigSource : public Serializable
{
public:
	std::string uri;
	std::string path;
	std::string workspace;

	void Deserialize(nlohmann::json json) override;
};

class EditorConfigUpdateParams : public Serializable
{
public:
	EditorConfigUpdateType type;
	EditorConfigSource source;

	void Deserialize(nlohmann::json json) override;
};

class DocumentRangeFormattingParams: public Serializable
{
public:
	TextDocument textDocument;
	Range range;

	void Deserialize(nlohmann::json json) override;
};

class TextDocumentPositionParams: public Serializable
{
public:
	TextDocument textDocument;
	Position position;

	void Deserialize(nlohmann::json json) override;
};

}
