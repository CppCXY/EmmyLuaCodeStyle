#pragma once

#include "LuaParser/LuaFile.h"
#include "LuaParser/LuaParser.h"
#include "CodeFormatServer/VSCode.h"

class VirtualFile
{
public:
	VirtualFile(std::string_view filename, std::string&& text);

	void UpdateFile(vscode::Range range, std::string& content);

	void UpdateFile(std::string& text);

	void Reset();

	std::shared_ptr<LuaParser> GetLuaParser();

private:
	void MakeParser();

	std::string _filename;
	std::shared_ptr<LuaFile> _luaFile;
	std::shared_ptr<LuaParser> _luaParser;
};