#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "VSCode.h"
#include "VSCode.h"
#include "VSCode.h"

// 命名风格和vscode一样

namespace vscode {

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
	uint64_t line;
	uint64_t character;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class Range : public Serializable
{
public:
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

class PublishDiagnosticsParams : public Serializable
{
public:
	std::string uri;

	std::vector<Diagnostic> diagnostics;

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

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class ServerCapabilities : public Serializable
{
public:
	TextDocumentSyncOptions textDocumentSync;
	bool documentFormattingProvider;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class InitializeParams : public Serializable
{
public:
	std::string rootUri;
	std::string rootPath;
	std::string locale;

	nlohmann::json initializationOptions;
	// 其他参数忽略

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};


class InitializeResult : public Serializable
{
public:
	ServerCapabilities capabilities;

	nlohmann::json Serialize() override;
	void Deserialize(nlohmann::json json) override;
};

class TextDocumentContentChangeEvent: public Serializable
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


}
