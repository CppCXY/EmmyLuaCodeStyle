#include "CodeFormatServer/VirtualFile/VirtualFile.h"

VirtualFile::VirtualFile(std::string_view filename, std::string&& text)
	: _filename(filename),
	  _luaFile(std::make_shared<LuaFile>(std::filesystem::path(filename).filename().string(), std::move(text)))
{
	MakeParser();
}

void VirtualFile::UpdateFile(vscode::Range range, std::string& content)
{
	// auto startLine = range.start.line;
	// if
}

void VirtualFile::UpdateFile(std::string& text)
{
	_luaFile = std::make_shared<LuaFile>(std::filesystem::path(_filename).filename().string(), std::move(text));
	MakeParser();
}

void VirtualFile::Reset()
{
}

std::shared_ptr<LuaParser> VirtualFile::GetLuaParser()
{
	return _luaParser;
}

void VirtualFile::MakeParser()
{
	auto tokenParser = std::make_shared<LuaTokenParser>(_luaFile);
	tokenParser->Parse();
	_luaParser = std::make_shared<LuaParser>(tokenParser);
	_luaParser->BuildAstWithComment();
}
