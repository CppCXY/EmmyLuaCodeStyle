#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>
#include "nlohmann/json.hpp"
#include <compare>
#include <optional>

// 命名风格和vscode一样

namespace lsp {
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

	template <class T>
	nlohmann::json SerializeArray(std::vector<T>& vec)
	{
		nlohmann::json array = nlohmann::json::array();

		for (auto& v : vec)
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
};

class Position : public Serializable
{
public:
	Position(std::size_t _line = 0, std::size_t _character = 0);

	std::size_t line;
	std::size_t character;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;

	bool operator==(const Position& p) const
	{
		return line == p.line && character == p.character;
	}

	bool operator<(const Position& p) const
	{
		if (line != p.line)
		{
			return line < p.line;
		}
		else
		{
			return character < p.character;
		}
	}

	bool operator<=(const Position& p) const
	{
		return *this == p || *this < p;
	}

	bool operator>=(const Position& p) const
	{
		return *this == p || !(*this < p);
	}
};

class Range : public Serializable
{
public:
	Range(Position _start = Position(0, 0), Position _end = Position(0, 0));
	Position start;
	Position end;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;

	bool operator==(const Range& range) const
	{
		return start == range.start && end == range.end;
	}

	bool operator<(const Range& rhs) const
	{
		if (start != rhs.start)
		{
			return start < rhs.start;
		}
		else
		{
			return end < rhs.end;
		}
	}
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
	std::string code;
	std::string data;
	std::string source;

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

class CompletionItemOptions : public Serializable
{
public:
	bool labelDetailsSupport = true;

	nlohmann::json Serialize() override;
};

class CompletionOptions : public Serializable
{
public:
	bool supportCompletion = false;
	std::vector<std::string> triggerCharacters;
	bool resolveProvider;
	CompletionItemOptions completionItem;

	nlohmann::json Serialize() override;
};

class DiagnosticOptions : public Serializable
{
public:
	std::string identifier;
	bool workspaceDiagnostics = false;
	bool interFileDependencies = false;

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
	CompletionOptions completionProvider;
	DiagnosticOptions diagnosticProvider;

	nlohmann::json Serialize() override;
};

class ConfigSource;

class InitializationOptions : public Serializable
{
public:
	std::vector<std::string> workspaceFolders;
	std::vector<ConfigSource> editorConfigFiles;
	std::vector<ConfigSource> moduleConfigFiles;
	std::string localeRoot;
	std::string extensionChars;
	std::vector<std::string> dictionaryPath;
    nlohmann::json vscodeConfig;
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

enum class FileChangeType
{
	Created = 1,
	Changed = 2,
	Delete = 3
};

class ConfigSource : public Serializable
{
public:
	std::string uri;
	std::string path;
	std::string workspace;

	void Deserialize(nlohmann::json json) override;
};

class ConfigUpdateParams : public Serializable
{
public:
	FileChangeType type;
	ConfigSource source;

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

class OptionalVersionedTextDocumentIdentifier : public TextDocument
{
public:
	std::optional<int> version;

	nlohmann::json Serialize() override;
};

class TextDocumentEdit : public Serializable
{
public:
	OptionalVersionedTextDocumentIdentifier textDocument;

	std::vector<TextEdit> edits;

	nlohmann::json Serialize() override;
};

class WorkspaceEdit : public Serializable
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

class FileEvent : public Serializable
{
public:
	std::string uri;
	FileChangeType type;

	void Deserialize(nlohmann::json json) override;
};

class DidChangeWatchedFilesParams : public Serializable
{
public:
	std::vector<FileEvent> changes;

	void Deserialize(nlohmann::json json) override;
};

class CompletionParams : public Serializable
{
public:
	TextDocument textDocument;

	Position position;

	void Deserialize(nlohmann::json json) override;
};

enum class CompletionItemKind
{
	Text = 1,
	Method = 2,
	Function = 3,
	Constructor = 4,
	Field = 5,
	Variable = 6,
	Class = 7,
	Interface = 8,
	Module = 9,
	Property = 10,
	Unit = 11,
	Value = 12,
	Enum = 13,
	Keyword = 14,
	Snippet = 15,
	Color = 16,
	File = 17,
	Reference = 18,
	Folder = 19,
	EnumMember = 20,
	Constant = 21,
	Struct = 22,
	Event = 23,
	Operator = 24,
	TypeParameter = 25
};

class CompletionItemLabelDetails : public Serializable
{
public:
	std::string detail;
	std::optional<std::string> description;

	nlohmann::json Serialize() override;
};

class CompletionItem : public Serializable
{
public:
	std::string label;
	CompletionItemKind kind;
	std::string detail;
	std::string documentation;
	std::string insertText;
	std::optional<CompletionItemLabelDetails> labelDetails;
	std::optional<std::string> sortText;
	std::optional<std::string> filterText;
	std::optional<Command> command;

	nlohmann::json Serialize() override;
};

class CompletionList : public Serializable
{
public:
	bool isIncomplete;
	std::vector<CompletionItem> items;

	nlohmann::json Serialize() override;
};

class DidChangeConfigurationParams : public Serializable
{
public:
	nlohmann::json settings;

	void Deserialize(nlohmann::json json) override;
};

class DocumentDiagnosticParams : public Serializable
{
public:
	TextDocument textDocument;
	std::string identifier;
	std::string previousResultId;

	void Deserialize(nlohmann::json json) override;
};

namespace DocumentDiagnosticReportKind {
static constexpr std::string_view Full = "full";
static constexpr std::string_view Unchanged = "unchanged";
};


class DocumentDiagnosticReport : public Serializable
{
public:
	std::string kind;

	std::string resultId;

	std::vector<Diagnostic> items;

	nlohmann::json Serialize() override;
};

class PreviousResultId : public Serializable
{
public:
	std::string uri;
	std::string value;

	void Deserialize(nlohmann::json json) override;
};

class WorkspaceDiagnosticParams : public Serializable
{
public:
	std::string identifier;

	std::vector<PreviousResultId> previousResultIds;

	void Deserialize(nlohmann::json json) override;
};

class WorkspaceDocumentDiagnosticReport : public DocumentDiagnosticReport
{
public:
	std::string uri;
	std::optional<int64_t> version;

	nlohmann::json Serialize() override;
};

class WorkspaceDiagnosticReport : public Serializable
{
public:
	std::vector<WorkspaceDocumentDiagnosticReport> items;

	nlohmann::json Serialize() override;
};

}
