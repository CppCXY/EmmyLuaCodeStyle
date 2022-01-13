#pragma once

#include "LuaParser/LuaFile.h"
#include "LuaParser/LuaParser.h"
#include "CodeFormatServer/VSCode.h"

class VirtualFile
{
public:
	VirtualFile(std::string_view fileUri);

	void UpdateFile(vscode::Range range, std::string&& content);

	void UpdateFile(std::vector<vscode::TextDocumentContentChangeEvent>& changeEvent);

	void UpdateFile(std::string&& text);

	void Reset();

	std::shared_ptr<LuaParser> GetLuaParser();

	void MakeParser();
private:


	std::string _fileUri;
	std::shared_ptr<LuaFile> _luaFile;
	std::shared_ptr<LuaParser> _luaParser;

	// bool _isOpen;
};