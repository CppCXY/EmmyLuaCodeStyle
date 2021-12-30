#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>
#include <nlohmann/json.hpp>
#if defined(__GNUC__) && __GNUC__ < 10
#else
#include <compare>
#endif
// 命名风格和vscode一样

namespace vscode
{
class Serializable
{
public:
	virtual ~Serializable();

	virtual nlohmann::json Serialize()
	{
		return nlohmann::json::object();
	}

	virtual void Deserialize(nlohmann::json json)
	{
	};

	template<class T>
	nlohmann::json SerializeArray(std::vector<T>& vec)
	{
		nlohmann::json array = nlohmann::json::array();

		for(auto& v: vec)
		{
			if constexpr (std::is_base_of_v<Serializable, T>)
			{
				array.push_back(v.Serialize());
			}
			else
			{
				array.push_back(v);
			}
			
		}
		return array;
	}

	auto operator<=>(const Serializable& rhl) const = default;
};

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

class ExecuteCommandOptions : public Serializable
{
public:
	std::vector<std::string> commands;

	nlohmann::json Serialize() override;
};

class ServerCapabilities : public Serializable
{
public:
	TextDocumentSyncOptions textDocumentSync;
	bool documentFormattingProvider = false;
	bool documentRangeFormattingProvider = false;
	DocumentOnTypeFormattingOptions documentOnTypeFormattingProvider;
	bool codeActionProvider = false;
	ExecuteCommandOptions executeCommandProvider;

	nlohmann::json Serialize() override;
};

class EditorConfigSource;

class InitializationOptions : public Serializable
{
public:
	std::vector<std::string> workspaceFolders;
	std::vector<EditorConfigSource> configFiles;
	std::string localeRoot;
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
	std::optional<Range> range;
	std::string text;

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

class DocumentRangeFormattingParams : public Serializable
{
public:
	TextDocument textDocument;
	Range range;

	void Deserialize(nlohmann::json json) override;
};

class TextDocumentPositionParams : public Serializable
{
public:
	TextDocument textDocument;
	Position position;

	void Deserialize(nlohmann::json json) override;
};

class CodeActionContext : public Serializable
{
public:
	std::vector<Diagnostic> diagnostics;

	void Deserialize(nlohmann::json json) override;
};

class CodeActionParams : public Serializable
{
public:
	TextDocument textDocument;

	Range range;

	CodeActionContext context;

	void Deserialize(nlohmann::json json) override;
};

class Command : public Serializable
{
public:
	std::string title;

	std::string command;

	std::vector<nlohmann::json> arguments;

	nlohmann::json Serialize() override;
};

class CodeActionKind
{
public:
	inline static std::string Empty = "";

	inline static std::string QuickFix = "quickfix";

	inline static std::string Refactor = "refactor";
};

class CodeAction : public Serializable
{
public:
	std::string title;

	Command command;

	std::string kind;

	nlohmann::json Serialize() override;
};

class CodeActionResult : public Serializable
{
public:
	std::vector<CodeAction> actions;

	nlohmann::json Serialize() override;
};

class ExecuteCommandParams : public Serializable
{
public:
	std::string command;

	std::vector<nlohmann::json> arguments;

	void Deserialize(nlohmann::json json) override;
};

class OptionalVersionedTextDocumentIdentifier: public TextDocument
{
public:
	std::optional<int> version;

	nlohmann::json Serialize() override;
};

class TextDocumentEdit: public Serializable
{
public:
	OptionalVersionedTextDocumentIdentifier textDocument;

	std::vector<TextEdit> edits;

	nlohmann::json Serialize() override;
};

class WorkspaceEdit: public Serializable
{
public:
    // std::vector<TextDocumentEdit> documentChanges;
	std::map<std::string, std::vector<TextEdit>> changes;
	nlohmann::json Serialize() override;
};


class ApplyWorkspaceEditParams : public Serializable
{
public:
	std::optional<std::string> label;
	WorkspaceEdit edit;

	nlohmann::json Serialize() override;
};
}
